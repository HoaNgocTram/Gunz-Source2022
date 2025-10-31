#include "stdafx.h"
#include "LottieWidget.h"
#include "MZFileSystem.h"
#ifdef _LOTTIE
#include <rlottie.h>

LottieWidget::LottieWidget(const char* szName, MWidget* pParent, MListener* pListener)
	: MWidget(szName, pParent, pListener)
{
}

void LottieWidget::OnDraw(MDrawContext* pDC)
{
	if (!Initialized)
		return;

	if (!animation)
		return;

	auto rect = GetRect();

	int x{}, y{};
	MRECT sr = GetScreenRect();
	x = sr.x + rect.x;
	y = sr.y + rect.y;

	//update animation and draw
	animation->RenderTexture(x, y, x + rect.w, y + rect.h);
}

void LottieWidget::LoadFilePath(std::string path)
{
	if (Initialized)
		return;

	MZFile File;
	if (!File.Open(path.c_str(), ZApplication::GetFileSystem()))
	{
		return;
	}

	int Length = File.GetLength();

	if (Length <= 0)
		return;

	char* buffer = new char[File.GetLength() + 1];
	buffer[File.GetLength()] = 0;

	File.Read(buffer, File.GetLength());

	animation = rlottie::Animation::loadFromData(buffer, path, path);

	if (!animation)
		return;

	filePath = path;
	Initialized = true;
}
#endif