﻿#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonJednostkiAlternatywneImport;

class CImportJednostkiAlternatywneD : public CMyDialog, public CCommonWTIDlgTPL<CCommonJednostkiAlternatywneImport>
{
    DECLARE_DYNAMIC(CImportJednostkiAlternatywneD)

public:
    CImportJednostkiAlternatywneD();   // standard constructor
    virtual ~CImportJednostkiAlternatywneD();

    // Dialog Data
    enum { IDD = IDD_JEDNOSTKIALKT_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonJednostkiAlternatywneImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyRadioButton m_ex_pomin;
    CMyRadioButton m_ex_actualize;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
