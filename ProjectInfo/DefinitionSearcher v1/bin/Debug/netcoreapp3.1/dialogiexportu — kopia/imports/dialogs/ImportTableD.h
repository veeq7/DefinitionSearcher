﻿#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonTableImport;

class CImportTableD : public CMyDialog, public CCommonWTIDlgTPL<CCommonTableImport>
{
    DECLARE_DYNAMIC(CImportTableD)

public:
    CImportTableD();   // standard constructor
    virtual ~CImportTableD();

    // Dialog Data
    enum { IDD = IDD_TABLE_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonTableImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    CMyEditCtrl m_tablename;

    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
