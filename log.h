#pragma once

#include <boost/log/trivial.hpp>

#if 0
#define DEF_ARGS __FILE__ << ":" << __LINE__ << "@" << __FUNCTION__ << " - "
#else
#define DEF_ARGS ""
#endif

#define LOG_TRACE() BOOST_LOG_TRIVIAL(trace) << DEF_ARGS
#define LOG_DEBUG() BOOST_LOG_TRIVIAL(debug) << DEF_ARGS
#define LOG_INFO() BOOST_LOG_TRIVIAL(info) << DEF_ARGS
#define LOG_WARNING() BOOST_LOG_TRIVIAL(warning) << DEF_ARGS
#define LOG_ERROR() BOOST_LOG_TRIVIAL(error) << DEF_ARGS
#define LOG_FATAL() BOOST_LOG_TRIVIAL(fatal) << DEF_ARGS
