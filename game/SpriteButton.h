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
#pragma once

class /*EXT_DECL*/ CSpriteButton : public CSprite
{
	bool m_bVisible, m_bSel, m_bHover, m_bEnabled;
	Uint16 m_nCmd;

	std::string m_text;
	Sint16 m_nPtSize;
	std::string m_strFont;
	CColor m_colorText, m_colorSelText, m_colorHoverText, m_colorDisabledText;
	CColor m_colorBack, m_colorSelBack, m_colorHoverBack, m_colorDisabledBack;
	CColor m_colorFrame, m_colorSelFrame, m_colorHoverFrame, m_colorDisabledFrame;

	CGraphics *m_pImage, *m_pSelImage, *m_pHoverImage, *m_pDisabledImage;

public:
	CSpriteButton(CVector pt, float w, float h, Uint16 nCmd, CColor colorText, CColor colorBack, std::string text, std::string strFont, Sint16 nPtSize, Uint32 time);
	CSpriteButton(float x, float y, float w, float h, Uint16 nCmd, CColor colorText, CColor colorBack, std::string text, std::string strFont, Sint16 nPtSize, Uint32 time);
	CSpriteButton(CRectangle rect, Uint16 nCmd, CColor colorText, CColor colorBack, std::string text, std::string strFont, Sint16 nPtSize, Uint32 time);
	~CSpriteButton();

	void SetCmd(Uint16 nCmd) { m_nCmd = nCmd; }
	const Uint16 GetCmd() { return m_nCmd; }

	CSpriteButton *Show(bool bVisible = true) { m_bVisible = bVisible; return this; }
	const bool IsVisible() { return m_bVisible; }

	CSpriteButton *Select(bool bSel = true) { m_bSel = bSel; InvalidateButton(); return this; }
	const bool IsSelected() { return m_bSel; }

	CSpriteButton *Hover(bool bHover = true) { m_bHover = bHover; InvalidateButton(); return this; }
	const bool IsHover() { return m_bHover; }

	CSpriteButton *Enable(bool bEnabled = true) { m_bEnabled = bEnabled; InvalidateButton(); return this; }
	const bool IsEnabled() { return m_bEnabled; }

	void SetText(std::string text, Sint16 nPtSize = 20, string strFont = "arial.ttf")	{ m_text = text; m_nPtSize = nPtSize; m_strFont = strFont; InvalidateButton(); }
	
	void SetTextColor(CColor color) { m_colorText = color; }
	void SetTextColor(CColor color, CColor colorSel, CColor colorHover, CColor colorDisabled) { m_colorText = color; m_colorSelText = colorSel; m_colorHoverBack = colorHover; m_colorDisabledBack = colorDisabled; InvalidateButton(); }
	void SetSelectedTextColor(CColor colorSel) { m_colorSelText = colorSel; InvalidateButton(); }
	void SetHoverTextColor(CColor colorHover) { m_colorHoverText = colorHover; InvalidateButton(); }
	void SetDisabledTextColor(CColor colorDisabled) { m_colorDisabledText = colorDisabled; InvalidateButton(); }

	void SetBackColor(CColor color) { m_colorText = color; InvalidateButton(); }
	void SetBackColor(CColor color, CColor colorSel, CColor colorHover, CColor colorDisabled) { m_colorBack = color; m_colorSelBack = colorSel; m_colorHoverBack = colorHover; m_colorDisabledBack = colorDisabled; InvalidateButton(); }
	void SetSelectedBackColor(CColor colorSel) { m_colorSelBack = colorSel; InvalidateButton(); }
	void SetHoverBackColor(CColor colorHover) { m_colorHoverBack = colorHover; InvalidateButton(); }
	void SetDisabledBackColor(CColor colorDisabled) { m_colorDisabledBack = colorDisabled; InvalidateButton(); }

	void SetFrameColor(CColor color) { m_colorFrame = color; InvalidateButton(); }
	void SetFrameColor(CColor color, CColor colorSel, CColor colorHover, CColor colorDisabled) { m_colorFrame = color; m_colorSelFrame = colorSel; m_colorHoverFrame = colorHover; m_colorDisabledBack = colorDisabled; InvalidateButton(); }
	void SetSelectedFrameColor(CColor colorSel) { m_colorSelFrame = colorSel; InvalidateButton(); }
	void SetHoverFrameColor(CColor colorHover) { m_colorHoverFrame = colorHover; InvalidateButton(); }
	void SetDisabledFrameColor(CColor colorDisabled) { m_colorDisabledFrame = colorDisabled; InvalidateButton(); }

	void SetDefaultColors(CColor colorText, CColor colorBack) {
		m_colorText = colorText; m_colorSelText = CColor::AnyBut(colorText); m_colorHoverText = m_colorSelText; m_colorDisabledText = CColor::DarkGray();
		m_colorBack = m_colorSelBack = m_colorHoverBack = m_colorDisabledBack = colorBack;
		m_colorFrame = m_colorDisabledFrame = m_colorText; m_colorSelFrame = m_colorHoverFrame = m_colorSelText; m_colorDisabledFrame = CColor::DarkGray();
	}

	void LoadImages(string strImage = "", string strSelImage = "", string strHoverImage = "", string strDisabledImage = "");
	void LoadImages(CGraphics *pImage = NULL, CGraphics *pSelImage = NULL, CGraphics *pHoverImage = NULL, CGraphics *pDisabledImage = NULL);

	void InvalidateButton();

protected:
	virtual void OnPrepareGraphics(CGraphics *pG = NULL);
	virtual void OnDraw(CGraphics *g);
};
