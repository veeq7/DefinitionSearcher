#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/n/klient/selectklientn.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonZlecenieImport;

class CImportDanychZleceniaD : public CMyDialog, public CCommonWTIDlgTPL<CCommonZlecenieImport>
{
    DECLARE_DYNAMIC(CImportDanychZleceniaD)

public:
    CImportDanychZleceniaD();   // standard constructor
    virtual ~CImportDanychZleceniaD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonZlecenieImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano, _ilePominieto;
};
