#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonPracownikImport;

class CImportPracownicyD : public CMyDialog, public CCommonWTIDlgTPL<CCommonPracownikImport>
{
    DECLARE_DYNAMIC(CImportPracownicyD)
    DECLARE_MESSAGE_MAP()

public:
    CImportPracownicyD();   // standard constructor
    virtual ~CImportPracownicyD();

    bool RegisterOnInit();

    INT_PTR DoModal();
    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonPracownikImport>& prm);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    virtual void OnOK();
    afx_msg void OnBnCsv();

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

private:
    int _ileZaimportowano;
};
