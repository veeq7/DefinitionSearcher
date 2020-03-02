#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonStrukturKonstrukcyjnychImport;
class CProcadStrukturyImport;

class CImportStrukturKonstrukcyjnychD
    : public CMyDialog
    , public CCommonWTIDlgTPL<CCommonStrukturKonstrukcyjnychImport>
    , public CCommonWTIDlgTPL<CProcadStrukturyImport>
{
    DECLARE_DYNAMIC(CImportStrukturKonstrukcyjnychD)
    DECLARE_MESSAGE_MAP()

public:
    CImportStrukturKonstrukcyjnychD();
    virtual ~CImportStrukturKonstrukcyjnychD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonStrukturKonstrukcyjnychImport>& prm);
    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CProcadStrukturyImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyRadioButton m_i_procadxml;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CMyCheckBox m_zaloz_kt;
    CMyCheckBox m_autozatwierdzanie;
    CMyCheckBox m_c_dwuetapowy; //IDC_C_DWUETAPOWY

    CMyRadioButton m_radio_aktualizacja;
    CMyRadioButton m_radio_wersja;
    CMyRadioButton m_radio_relacja;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

private:
    int _ileZaimportowano;
};
