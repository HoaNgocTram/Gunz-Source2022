#pragma once

#include "MWidget.h"
#ifdef _LOTTIE
class ZIDLResource;

namespace rlottie { class Animation; }

class LottieWidget : public MWidget {
protected:
	std::string filePath;
	bool Initialized{};

	virtual void OnDraw(MDrawContext* pDC);

	void LoadFilePath(std::string path);

	std::unique_ptr<rlottie::Animation> animation;

	friend class ZIDLResource;
public:
	LottieWidget(const char* szName = NULL, MWidget* pParent = NULL, MListener* pListener = NULL);

#define MINT_LOTTIE "Lottie"
	virtual const char* GetClassName(void) { return MINT_LOTTIE; }
};
#endif