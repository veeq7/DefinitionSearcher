#pragma once
#include "wtih.h"
#include "../CommonMiejsceMagazynoweImport.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/selects/selectmagazyn.h"

class CCommonMiejscaMagazynoweImport;

class CImportMiejscaMagazynoweD
    : public CMyDialog
    , public CCommonWTIDlgTPL<CCommonMiejsceMagazynoweImport>
{
    DECLARE_DYNAMIC(CImportMiejscaMagazynoweD)

public:
    CImportMiejscaMagazynoweD();   // standard constructor
    virtual ~CImportMiejscaMagazynoweD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonMiejsceMagazynoweImport>& prm);
    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;

protected:
    CSelectMagazyn m_tmg_idmagazynu;
};
