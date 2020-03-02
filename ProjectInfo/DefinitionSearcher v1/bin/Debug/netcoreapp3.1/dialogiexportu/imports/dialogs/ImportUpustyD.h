#pragma once
#include "../../common.h"

#include "wtih.h"
//DIALOGIGENEROWAN
#include "../../hs/const.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
#include "imports/dialogs/ImportWartosciDowD.h"
//Resources
#include "../../resource.h"

class CCommonImportDataProvider;
class CImportDataProviderC;

class CImportUpustyD : public CMyDialog, public CCommonWTIDlgTPL<CImportDataProviderC>
{
    DECLARE_DYNAMIC(CImportUpustyD)

public:
    CImportUpustyD();   // standard constructor
    virtual ~CImportUpustyD();

    // Dialog Data
    enum { IDD = IDD_UPUSTY_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    eHowActualize getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CImportDataProviderC>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel, m_i_cfixml;
    CMyRadioButton m_i_cficsv;
    CMyRadioButton m_ex_pomin;
    CMyRadioButton m_ex_actualize;
    CButtonST m_csv_params;
    CMyComboCtrl m_map_kod;

    CMyRadioButton m_ci_sprzedazy;
    CMyRadioButton m_ci_zakupu;

    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CCommonCSVConfigC csvconf;

    CCommonImportDataProvider* getDataProvider();

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
