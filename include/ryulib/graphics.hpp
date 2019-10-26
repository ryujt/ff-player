#ifndef RYULIB_GRAPHICS_HPP
#define RYULIB_GRAPHICS_HPP

#include <windows.h>
#include <Winuser.h>
#include <vector>

using namespace std;

static vector<RECT> monitors;

static int get_monitor_count()
{
	return GetSystemMetrics(SM_CMONITORS);
}

static BOOL CALLBACK EnumDisplayMonitorsCallBack(HMONITOR monitor, HDC hdc, LPRECT pRect, LPARAM dwData)
{
	vector<RECT> *pMonitors = (vector<RECT> *) dwData;

	MONITORINFOEX monitor_info;
	monitor_info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(monitor, &monitor_info);

#ifdef _DEBUG
	printf("monitor_info.rcMonitor.left: %d \n", monitor_info.rcMonitor.left);
	printf("monitor_info.rcMonitor.top: %d \n", monitor_info.rcMonitor.top);
	printf("monitor_info.rcMonitor.right: %d \n", monitor_info.rcMonitor.right);
	printf("monitor_info.rcMonitor.bottom: %d \n", monitor_info.rcMonitor.bottom);
#endif

	pMonitors->push_back(monitor_info.rcMonitor);

	return TRUE;
}

static RECT get_monitor_rect(int no)
{
	if (monitors.size() == 0) {
		EnumDisplayMonitors(NULL, NULL, &EnumDisplayMonitorsCallBack, (LPARAM)&monitors);
	}

	if (monitors.size() == 0) {
		return RECT {0, 0, 0, 0};
	}

	return monitors.at(no);
}

static int get_monitor_width(int no)
{
	if (monitors.size() == 0) {
		EnumDisplayMonitors(NULL, NULL, &EnumDisplayMonitorsCallBack, (LPARAM) &monitors);
	}

	if (monitors.size() == 0) return 0;

	RECT rect = get_monitor_rect(no);

	return abs(rect.right - rect.left);
}

static int get_monitor_height(int no)
{
	if (monitors.size() == 0) {
		EnumDisplayMonitors(NULL, NULL, &EnumDisplayMonitorsCallBack, (LPARAM) &monitors);
	}

	if (monitors.size() == 0) return 0;

	RECT rect = get_monitor_rect(no);

	return abs(rect.bottom - rect.top);
}

#endif // RYULIB_GRAPHICS_HPP 