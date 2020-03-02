#pragma once
#include "../../common.h"

#include "wtih.h"
//dialogiexportu
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonRozchodTechnolImport;

class CImportRozchodTechnolD : public CMyDialog, public CCommonWTIDlgTPL<CCommonRozchodTechnolImport>
{
    DECLARE_DYNAMIC(CImportRozchodTechnolD)

public:
    CImportRozchodTechnolD(const CDTTypeID& main_tid, const CMyString& elem_id, const eRROZCHODU& rrozchodu = RROZCHOD_UNKNOWN);   // standard constructor
    virtual ~CImportRozchodTechnolD();

    virtual BOOL OnInitDialog();
    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();
    afx_msg void OnBnCsv();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonRozchodTechnolImport>& prm);

    INT_PTR DoModal();

protected:
    CMyString getRegSaveDataTStr(const CDTTypeID& main_tid, const CMyString& elem_id, const eRROZCHODU& rrozchodu);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

    CMyFileEditCtrl m_fileName;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CMyCheckBox m_im_crete_towar;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyRadioButton m_r_rozchod;
    CMyRadioButton m_r_przychod;
    CMyComboCtrl m_map_kod;
    CButtonST m_csv_params;
    CButtonST m_ok;
    CButtonST m_cancel;

    CCommonCSVConfigC csvconf;

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;

    CDTTypeID main_tid;
    CMyString elem_id;
    eRROZCHODU rrozchodu;
};
