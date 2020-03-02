#include "StdAfx.h"
#include "managerdll.h"
#include "ImportPlanKontD.h"

#include "wti.h"
#include "../excel/excelplankontimport.h"
#include "../CommonPlanKontImport.h"
#include "../csv/CSVPlanKontImport.h"
#include "dialogs/male/CSVParametryD.h"
//Szczegolowe

// CImportPlanKontD dialog

IMPLEMENT_DYNAMIC(CImportPlanKontD, CMyDialog)
CImportPlanKontD::CImportPlanKontD()
: CMyDialog(CImportPlanKontD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\PlanKont"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));

    m_ex_pomin.addRadio(&m_ex_pomin, _T("SKIP"));
    m_ex_pomin.addRadio(&m_ex_actualize, _T("UPDATE"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_onexists"), _T("SKIP"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_PlanyKont, true);
}

CImportPlanKontD::~CImportPlanKontD()
{
    saveRegSaveDataT();
}

void CImportPlanKontD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_EX_POMIN, m_ex_pomin);
    DDX_Control(pDX, IDC_EX_ACTUALIZE, m_ex_actualize);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportPlanKontD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_onexists", m_ex_pomin);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportPlanKontD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportPlanKontD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportPlanKontD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}



class CImportPlanKontD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportPlanKontD_DM);
};


DIALOGMANAGER_REGISTERT("ImportPlanKontD", CImportPlanKontD_DM);


bool CImportPlanKontD_DM::executeForParam(const CMyString& param)
{
    CImportPlanKontD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportPlanKontD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


void CImportPlanKontD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonPlanKontImport* pk = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) pk = new CExcelPlanKontImport(YKH_KH);
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        pk = new CCSVPlanKontImport(YKH_KH);
        if (pk) ((CCSVPlanKontImport*)pk)->setCSVConfig(csvconf);
    }

    pk->setHowActualize(getHowActualize());
    pk->setFileName(DTGDTGETPOLET("_fileName"));
    pk->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    pk->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, pk, 0);

    if (pk->isError())
    {
        AfxMultiMessageBox(pk->getErrorMessage());
        DELETE_WSK(pk);
        return;
    }

    DELETE_WSK(pk);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" kont"));

    CMyDialog::OnOK();
}

void CImportPlanKontD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonPlanKontImport>& prm)
{
    CCommonPlanKontImport* eki = prm.getParam();
    if (eki)
    {
        eki->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s planu kont z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eki->execute();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eki->clearStatusRetHandler();
    }
}



int CImportPlanKontD::getHowActualize()
{
    if (DTGDTGETPOLET("_onexists") == _T("UPDATE")) return ACTUALIZE_UPDATE;
    return ACTUALIZE_SKIP;
}
// CImportPlanKontD message handlers

CUInfo CImportPlanKontD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportPlanKontD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_PLANKONT));
}

INT_PTR CImportPlanKontD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_PLANKONT))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
