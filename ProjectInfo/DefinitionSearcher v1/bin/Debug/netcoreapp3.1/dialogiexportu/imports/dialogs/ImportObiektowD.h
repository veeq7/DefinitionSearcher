#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonObiektowImport;

class CImportObiektowD : public CMyDialog, public CCommonWTIDlgTPL<CCommonObiektowImport>
{
    DECLARE_DYNAMIC(CImportObiektowD)

public:
    CImportObiektowD();   // standard constructor
    virtual ~CImportObiektowD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonObiektowImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyCheckBox m_im_wartoscidowolnych;
    CMyCheckBox m_im_laczenieobiektow_dokprzyj;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
