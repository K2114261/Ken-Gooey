/****************************************************************************
GFC - Games Fundamental Classes
Copyright (C) 2009-2016 Jarek Francik, Kingston University London

This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License:
http://creativecommons.org/licenses/by-sa/4.0/

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Created by Jarek Francik
jarek@kingston.ac.uk
****************************************************************************/
#include "stdafx.h"
//#include "..\GFC\include\GFC.h"
//#include "..\GFC\include\SpriteButton.h"
#include "SpriteButton.h"

CSpriteButton::CSpriteButton(CVector pt, float w, float h, Uint16 nCmd, CColor colorText, CColor colorBack, std::string text, std::string strFont, Sint16 nPtSize, Uint32 time)
	: CSprite(pt, w, h, time), m_bVisible(true), m_bSel(false), m_bHover(false), m_bEnabled(true), m_nCmd(nCmd), m_pImage(NULL), m_pSelImage(NULL), m_pHoverImage(NULL), m_pDisabledImage(NULL)
{
	SetText(text, nPtSize, strFont); SetDefaultColors(colorText, colorBack);
}
CSpriteButton::CSpriteButton(float x, float y, float w, float h, Uint16 nCmd, CColor colorText, CColor colorBack, std::string text, std::string strFont, Sint16 nPtSize, Uint32 time)
	: CSprite(x, y, w, h, time), m_bVisible(true), m_bSel(false), m_bHover(false), m_bEnabled(true), m_nCmd(nCmd), m_pImage(NULL), m_pSelImage(NULL), m_pHoverImage(NULL), m_pDisabledImage(NULL)
{
	SetText(text, nPtSize, strFont); SetDefaultColors(colorText, colorBack);
}
CSpriteButton::CSpriteButton(CRectangle rect, Uint16 nCmd, CColor colorText, CColor colorBack, std::string text, std::string strFont, Sint16 nPtSize, Uint32 time)
	: CSprite(rect, time), m_bVisible(true), m_bSel(false), m_bHover(false), m_bEnabled(true), m_nCmd(nCmd), m_pImage(NULL), m_pSelImage(NULL), m_pHoverImage(NULL), m_pDisabledImage(NULL)
{
	SetText(text, nPtSize, strFont); SetDefaultColors(colorText, colorBack);
}

CSpriteButton::CSpriteButton::~CSpriteButton()
{
}

void CSpriteButton::InvalidateButton()
{
	ClearImage();
}

void CSpriteButton::LoadImages(string strImage, string strSelImage, string strHoverImage, string strDisabledImage)
{
	if (m_pImage) delete m_pImage; m_pImage = strImage.empty() ? NULL : new CGraphics(strImage);
	if (m_pSelImage) delete m_pSelImage; m_pSelImage = strSelImage.empty() ? NULL : new CGraphics(strSelImage);
	if (m_pHoverImage) delete m_pHoverImage; m_pHoverImage = strHoverImage.empty() ? NULL : new CGraphics(strHoverImage);
	if (m_pDisabledImage) delete m_pDisabledImage; m_pDisabledImage = strDisabledImage.empty() ? NULL : new CGraphics(strDisabledImage);
}

void CSpriteButton::LoadImages(CGraphics *pImage, CGraphics *pSelImage, CGraphics *pHoverImage, CGraphics *pDisabledImage)
{
	if (m_pImage) delete m_pImage; m_pImage = pImage == NULL ? NULL : new CGraphics(pImage);
	if (m_pSelImage) delete m_pSelImage; m_pSelImage = pSelImage == NULL ? NULL : new CGraphics(pSelImage);
	if (m_pHoverImage) delete m_pHoverImage; m_pHoverImage = pHoverImage == NULL ? NULL : new CGraphics(pHoverImage);
	if (m_pDisabledImage) delete m_pDisabledImage; m_pDisabledImage = pDisabledImage == NULL ? NULL : new CGraphics(pDisabledImage);
}


void CSpriteButton::OnPrepareGraphics(CGraphics *pG)
{
	if (m_pGraphics) return;
	if (!pG) return;
	if (!GetWidth() || !GetHeight()) return;

	CGraphics *pImage = IsEnabled() ? (IsSelected() ? m_pSelImage : (IsHover() ? m_pHoverImage : m_pImage)) : m_pDisabledImage;
	if (pImage)
	{
		m_pGraphics = new CGraphics((int)GetWidth(), (int)GetHeight(), 32, 0xff0000, 0xff00, 0xff, 0);
		m_pGraphics->Blit(CVectorI((m_pGraphics->GetWidth() - pImage->GetWidth()) / 2, (m_pGraphics->GetHeight() - pImage->GetHeight()) / 2), *pImage);
	}
	else
	{
		CColor colorText = IsEnabled() ? (IsSelected() ? m_colorSelText : (IsHover() ? m_colorHoverText : m_colorText)) : m_colorDisabledText;
		CColor colorBack = IsEnabled() ? (IsSelected() ? m_colorSelBack : (IsHover() ? m_colorHoverBack : m_colorBack)) : m_colorDisabledBack;
		CColor colorFrame = IsEnabled() ? (IsSelected() ? m_colorSelFrame : (IsHover() ? m_colorHoverFrame : m_colorFrame)) : m_colorDisabledFrame;

		CRectangle rect;
		m_pGraphics = new CGraphics((int)GetWidth(), (int)GetHeight(), 32, 0xff0000, 0xff00, 0xff, 0);
		SetColorKey(CColor(1, 1, 1));
		GetClientRect(rect);
		m_pGraphics->FillRect(rect, colorBack);
		rect.Grow(0, -1, 0, -1);
		m_pGraphics->DrawRect(rect, colorFrame);

		if (m_text.empty()) return;

		// store pG settings
		std::string fface = pG->GetFontName();
		int fsize = pG->GetFontSize();
		CColor tclr = pG->GetTextColor();

		*pG << font(m_strFont, m_nPtSize);
		*pG << color(colorText);
		CGraphics *pTxt = pG->GetTextGraphics(m_text);
		m_pGraphics->Blit(CVectorI((m_pGraphics->GetWidth() - pTxt->GetWidth()) / 2, (m_pGraphics->GetHeight() - pTxt->GetHeight()) / 2 ), *pTxt);
		delete pTxt;
	
		// restore pG settings
		*pG << font(fface, fsize);
		*pG << color(tclr);
	}
}

void CSpriteButton::OnDraw(CGraphics*)
{
	// do nothing - the bitmap is already loaded in OnPrepareGraphics
}