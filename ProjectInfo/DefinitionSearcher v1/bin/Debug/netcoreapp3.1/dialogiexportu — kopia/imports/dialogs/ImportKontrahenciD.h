#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonKlientImport;

class CImportKontrahenciD : public CMyDialog, public CCommonWTIDlgTPL<CCommonKlientImport>
{
    DECLARE_DYNAMIC(CImportKontrahenciD)

public:
    CImportKontrahenciD();   // standard constructor
    virtual ~CImportKontrahenciD();

    // Dialog Data
    enum { IDD = IDD_KONTRAHENCI_IMPORT };

    bool RegisterOnInit();

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonKlientImport>& prm);

    INT_PTR DoModal();
    int getHowActualize();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    afx_msg void OnBnCsv();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CMyFileEditCtrl  m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyRadioButton m_ex_pomin;
    CMyRadioButton m_ex_actualize;
    CButtonST m_csv_params;
    CMyCheckBox m_im_wartoscidowolnych;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CCommonCSVConfigC csvconf;

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
