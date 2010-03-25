#pragma once
#ifndef STDAFX_H__
#define STDAFX_H__

#include <QtGui>
#include <QMetaType>

#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>

typedef boost::mutex              mutex_t;
typedef boost::mutex::scoped_lock lock_t;
typedef boost::condition_variable condition_t;

#define QT_FATAL_WARNINGS 1

#ifdef _WIN32
#include <windows.h>
#endif

#endif // STDAFX_H__
