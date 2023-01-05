#pragma once

#include <boost/mysql.hpp>
#include <mysqlConn/mysqlConnMgr.h>

#include "sessionManager.h"

namespace mlnserver::httpCoro {

	class MysqlFailedRead : public std::runtime_error
	{
	public:
		MysqlFailedRead(const std::string& _Message)
			: std::runtime_error(_Message)
		{}
	};



	// TLS Variables
	thread_local inline static boost::mysql::error_info additional_info;
	thread_local inline static std::string paramStringForDev;

	static void check_error(const boost::mysql::error_code& err, const boost::mysql::error_info& info = {})
	{
		if (err)
			throw boost::system::system_error(err, info.message());
	}



	static void sendQuery(MBMySQL::ConnPtr connObj, mln::net::send_lambda& sender, std::string& sql, boost::mysql::tcp_ssl_resultset& result)
	{
		connObj->async_query(sql, result, additional_info, sender.yield_[sender.ec_]);
		check_error(sender.ec_, additional_info);
	}


	static bool readOne(mln::net::send_lambda& sender, boost::mysql::tcp_ssl_resultset& result, boost::mysql::row& row)
	{
		const bool readResult = result.async_read_one(row, additional_info, sender.yield_[sender.ec_]);
		check_error(sender.ec_, additional_info);
		return readResult;
	}

	static void readAll(mln::net::send_lambda& sender, boost::mysql::tcp_ssl_resultset& result, boost::mysql::rows& rows)
	{
		result.async_read_all(rows, sender.yield_[sender.ec_]);
		check_error(sender.ec_, additional_info);
	}

	static void purgeResult(mln::net::send_lambda& sender, boost::mysql::tcp_ssl_resultset& result)
	{
		boost::mysql::row row;
		while (false == result.complete()) {
			if (false == readOne(sender, result, row)) {
				break;
			}
		}
	}




	static
	boost::beast::http::response<boost::beast::http::string_body>
		getBadRequest(const bool keep_alive, const unsigned int version, boost::beast::string_view why)
	{
		boost::beast::http::response<boost::beast::http::string_body> res{ boost::beast::http::status::bad_request, version };
		res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
		res.set(boost::beast::http::field::content_type, "application/json");
		res.keep_alive(keep_alive);
		res.body() = std::string(why);
		res.prepare_payload();
		return res;
	}

	static 
	boost::beast::http::response<boost::beast::http::string_body>
		getServerError(const bool keep_alive, const unsigned int version, boost::beast::string_view why)
	{
		boost::beast::http::response<boost::beast::http::string_body> res{ boost::beast::http::status::internal_server_error, version };
		res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
		res.set(boost::beast::http::field::content_type, "application/json");
		res.keep_alive(keep_alive);
		res.body() = std::string(why);
		res.prepare_payload();
		return res;
	}


	static
	boost::beast::http::response<boost::beast::http::string_body>
		getRsponse(boost::beast::string_view msg, const bool keep_alive, const unsigned int version, const std::string & bodyMsg = "0")
	{
		boost::beast::http::response<boost::beast::http::string_body> res{
			std::piecewise_construct,
			std::make_tuple(std::move(std::string(msg))),
			std::make_tuple(boost::beast::http::status::ok, version) };

		res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
		res.set(boost::beast::http::field::content_type, "application/json");
		res.set("mbbodyresult", bodyMsg);

		res.content_length(res.body().length());
		res.keep_alive(keep_alive);

		return res;
	}


	static
	void mysqlTemplate(
		mln::net::HeaderMap&& header,
		std::string&& bodyString,
		mln::net::send_lambda& sender,
		bool keep_alive,
		unsigned int version,
		std::optional<SessionManager::SessionData> sessionDataOpt,
		std::function<\
		void(mln::net::HeaderMap&& header, boost::json::value& jv, mln::net::send_lambda& sender\
			, bool keep_alive, unsigned int version\
			, std::optional<SessionManager::SessionData> sessionDataOpt)> cb
	)
	{
		LOGT("<restsvc:op> request url:{} requestBody:{}", header["__path__"], bodyString);

		std::error_code parsingError;
		boost::json::value jsonObj = boost::json::parse(bodyString, parsingError);
		if (parsingError) {
			SEND_INE(getBadRequest(keep_alive, version, R"({"msg":"invalid json-string"})"));
			return;
		}

		try {
			cb(std::move(header), jsonObj, sender, keep_alive, version, sessionDataOpt);
		}
		catch (const std::out_of_range& e) {
			LOGE("invalid json paramter. err:{}", e.what());

#ifdef METABOLOTS_USE_PARAM_CHECKER
			SEND_INE(getBadRequest(keep_alive, version
				, fmt::format("{{\"msg\":\"bad parameter:{}\"}}", paramStringForDev)
			));
#else
			SEND_INE(getBadRequest(keep_alive, version, R"({"msg":"bad parameter"})"));
#endif

		}
		catch (const MysqlFailedRead& e) {
			LOGE("Failed BoostMySQL. FailedRead. msg:{}", e.what());
			SEND_INE(getServerError(keep_alive, version, R"({"msg":"server error"})"));
		}
		catch (const boost::mysql::bad_field_access& e) {
			LOGE("Failed BoostMySQL. Bad Field Access");
			SEND_INE(getServerError(keep_alive, version, R"({"msg":"server error"})"));
		}
		catch (const boost::system::system_error& e) {
			LOGE("Failed BoostMySQL. err:{}", e.what());
			SEND_INE(getServerError(keep_alive, version, R"({"msg":"server error"})"));
		}
		catch (const std::exception e) {
			LOGE("failed mysql. err:{}", e.what());


#ifdef METABOLOTS_USE_PARAM_CHECKER
			SEND_INE(getBadRequest(keep_alive, version
				, fmt::format("{{\"msg\":\"bad parameter:{}\"}}", paramStringForDev)
			));
#else
			SEND_INE(getServerError(keep_alive, version, R"({"msg":"bad parameter"})"));
#endif
		}
	}

	inline static
	void queryInsertAndCheck(
		mln::net::send_lambda& sender
		, boost::mysql::tcp_ssl_resultset& result
		, MBMySQL::ConnPtr connObj
		, std::string &sql
		, const std::uint64_t attachedCnt
	)
	{
		purgeResult(sender, result);
		sendQuery(connObj, sender, sql, result);
		if (attachedCnt != result.affected_rows()) {
			// 기대한 레코드만큼 입력이 되지 않았습니다. 확인이 필요합니다.
			LOGE("expected value:{}row, result value:{}row, srcQuery:{}"
				, attachedCnt, result.affected_rows(), sql);
		}
	}


}//namespace mlnserver::httpCoro {
