#pragma once
#include "wtih.h"
#include "../../selects/SelectDoCzegoWartoscDowolna.h"
//Szczegolowe
#include "../selectscrm/selects/multivalues/SelectWartosciDowolne.h"
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"

class CCommonWartDowolneImport;

class CImportWartosciDowD : public CMyDialog, public CCommonWTIDlgTPL<CCommonWartDowolneImport>
{
    DECLARE_DYNAMIC(CImportWartosciDowD)

public:
    CImportWartosciDowD();   // standard constructor
    virtual ~CImportWartosciDowD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* src, CCommonWTIParamTPL<CCommonWartDowolneImport>& prm);

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

    CSelectDoCzegoWartoscDowolna m_rodzaj;
    CSelectWartosciDowolne m_wartosc;

    CMyComboCtrl m_mapby;

    bool changeMapBy(const eDataType& newDataType);

    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
    eDataType m_currentMapBy;
};
