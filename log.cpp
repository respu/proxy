//
//            Copyright (c) Marco Amorim 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
#include "log.h"

#include <map>
#include <fstream>
#include <sstream>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/from_stream.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace trivial = boost::log::trivial;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)

void init_log_system(
        const std::string& log_cfg,
        const std::string& log_level)
{
    std::map<std::string, trivial::severity_level> severity_map;
    severity_map["trace"] = trivial::trace;
    severity_map["debug"] = trivial::debug;
    severity_map["info"] = trivial::info;
    severity_map["warning"] = trivial::warning;
    severity_map["error"] = trivial::error;
    severity_map["fatal"] = trivial::fatal;

    logging::add_common_attributes();

    if (!log_cfg.empty())
    {
        std::ifstream settings(log_cfg.c_str());
        if (!settings.is_open())
        {
            std::ostringstream errm;
            errm << "could not open " << log_cfg << " file";
            throw std::invalid_argument(errm.str());
        }

        logging::init_from_stream(settings);
    }
    else
    {
        logging::add_console_log(
            std::clog,
            keywords::format =
            (
                expr::stream
                    << expr::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S.%f")
                    << ": <" << severity
                    << "> [" << channel << "] "
                    << expr::smessage
            )
        );
    }

    logging::core::get()->set_filter
    (
        trivial::severity >= severity_map[log_level]
    );
}
