#pragma once
#include "../../common.h"

#include "wtih.h"
//DIALOGIGENEROWAN
#include "../../hs/const.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonImportDataProvider;
class CImportDataProviderC;
class CProcadTowaryImport;

class CImportTowaryD 
    : public CMyDialog
    , public CCommonWTIDlgTPL<CProcadTowaryImport>
    , public CCommonWTIDlgTPL<CImportDataProviderC>
{
    DECLARE_DYNAMIC(CImportTowaryD)

public:
    CImportTowaryD(bool isCennikImportMode);   // standard constructor
    virtual ~CImportTowaryD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    eHowActualize getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CProcadTowaryImport>& prm);
    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CImportDataProviderC>& prm);

    INT_PTR DoModal();

    afx_msg void OnBnCsv();

    BOOL OnInitDialog();

protected:
    void onProcadImport();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cfixml;
    CMyRadioButton m_i_cficsv;
    CMyRadioButton m_i_procadxml;
    CMyRadioButton m_ex_pomin;
    CMyRadioButton m_ex_actualize;
    CMyRadioButton m_ex_onlyceny;
    CMyRadioButton m_ex_tylkoaktualizacja;
    CMyCheckBox m_im_wartoscidowolnych;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CButtonST m_csv_params;
    bool _cennikImportMode;

    CCommonCSVConfigC csvconf;

    CCommonImportDataProvider* getDataProvider();

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
