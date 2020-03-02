#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CKsiegowanieWzorcemBase;

class CImportKsiegowaniaD : public CMyDialog, public CCommonWTIDlgTPL<CKsiegowanieWzorcemBase>
{
    DECLARE_DYNAMIC(CImportKsiegowaniaD)

public:
    CImportKsiegowaniaD(const CMyString& zk_idzapisu);   // standard constructor
    virtual ~CImportKsiegowaniaD();

    // Dialog Data
    enum { IDD = IDD_IMPORT_KSIEGOWANIA };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    afx_msg void OnBnCsv();

    int getHowActualize();
    CMyString getFileMask();

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CKsiegowanieWzorcemBase>& prm);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;

    CButtonST m_csv_params;
    CButtonST m_ok;
    CButtonST m_cancel;

    CCommonCSVConfigC csvconf;

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
    CString zk_idzapisu;
};
