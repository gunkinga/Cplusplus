/***********************************************************************
功能：驱动级按键模拟
参数：无
返回值：无
注意：部分游戏或者应用会忽略keybd_event()第一个参数or第二个参数，
只要两个都填上参数即可，有些应用和游戏只需要一个参数。
已测试游戏 data2 可只用第二个参数 or 全部都用
***********************************************************************/
void keyevent(void)
{
	Sleep(5000);
	keybd_event(0x51, MapVirtualKey(0x51, 0), 0, 0);
	Sleep(300);
	keybd_event(0x51, MapVirtualKey(0x51, 0), KEYEVENTF_KEYUP, 0);
}
/***********************************************************************
功能：保存24位无损bmp位图文件
参数：路径
返回值：成功 or 失败
***********************************************************************/
BOOL WINAPI Picture::SaveBitmap(const char* Path)
{
	BITMAPFILEHEADER bfHeader;
	BITMAPINFOHEADER biHeader;
	BITMAPINFO bInfo;
	HGDIOBJ hTempBitmap;
	HBITMAP hBitmap;
	BITMAP bAllDesktops;
	HDC hDC, hMemDC;
	LONG lWidth, lHeight;
	BYTE *bBits = NULL;
	HANDLE hHeap = GetProcessHeap();
	DWORD cbBits, dwWritten = 0;
	HANDLE hFile;
	INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

	ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
	ZeroMemory(&bInfo, sizeof(BITMAPINFO));
	ZeroMemory(&bAllDesktops, sizeof(BITMAP));

	hDC = GetDC(NULL);
	hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
	GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

	lWidth = bAllDesktops.bmWidth;
	lHeight = bAllDesktops.bmHeight;

	DeleteObject(hTempBitmap);

	bfHeader.bfType = (WORD)('B' | ('M' << 8));
	bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	biHeader.biSize = sizeof(BITMAPINFOHEADER);
	biHeader.biBitCount = 24;
	biHeader.biCompression = BI_RGB;
	biHeader.biPlanes = 1;
	biHeader.biWidth = lWidth;
	biHeader.biHeight = lHeight;

	bInfo.bmiHeader = biHeader;

	cbBits = (((24 * lWidth + 31)&~31) / 8) * lHeight;

	hMemDC = CreateCompatibleDC(hDC);
	hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID **)&bBits, NULL, 0);
	SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);


	hFile = CreateFileA(Path, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);

	CloseHandle(hFile);

	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDC);
	DeleteObject(hBitmap);

	return TRUE;
}
/***********************************************************************
功能：二值化图像函数
参数：图片路径
返回值：无
***********************************************************************/
void Picture::BinarizationPicture(const char* Path)
{
	Mat Img = imread(Path);
	cvtColor(Img, RetVlu, COLOR_BGR2GRAY);
	threshold(RetVlu, GrayImg, 127, 255, THRESH_BINARY);
	imwrite("text.jpg", GrayImg);
}
/***********************************************************************
功能：模板匹配函数
参数：图片路径
返回值：匹配度
***********************************************************************/
double Picture::MatchFind(const char* Path_temp)
{
	Mat templ, result, Pic;
	templ = imread(Path_temp);
	Pic = imread("text.jpg");
	maxVal = 0;
	/*
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);
	*/
	//进行匹配和标准化
	matchTemplate(Pic, templ, result, TM_CCOEFF_NORMED);
	//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/*double minVal;  //匹配最小值
	double maxVal;  //匹配最大值
	Point maxLoc;  //匹配最大值的位置坐标
	Point minLoc;   //匹配最小值的位置坐标
	Point matchLoc;*/
	//通过函数minMaxLoc定位最匹配的位置

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = maxLoc;
	//cout << "匹配度:" << (maxVal * 100.0f) <<"%"<< endl;
	//cout << "匹配度最小值的坐标" << minLoc.x << "," << minLoc.y << endl;
	//cout << "匹配度最大值的坐标" << maxLoc.x << "," << maxLoc.y << endl;

	/*rectangle(Pic, Point(matchLoc.x + templ.rows, matchLoc.y + templ.cols),
		matchLoc,
		Scalar(0, 255, 0), 2, 8, 0);*/

		//imshow("img", Pic);
		//imshow("templ", templ);
		//imshow("result", result);

	return (maxVal*100.0f);
}