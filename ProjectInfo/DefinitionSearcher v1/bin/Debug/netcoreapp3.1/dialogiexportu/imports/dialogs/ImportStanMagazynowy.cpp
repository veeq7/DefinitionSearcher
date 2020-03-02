#include "StdAfx.h"
#include "managerdll.h"
#include "ImportStanMagazynowy.h"

#include "../CommonStanMagazynowyImport.h"
#include "../csv/CSVStanyMagazynoweImport.h"
#include "../excel/excelstanymagazynoweimport.h"
#include "../csv/CSVCennikImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
//Szczegolowe


// CImportStanMagazynowyD dialog

IMPLEMENT_DYNAMIC(CImportStanMagazynowyD, CMyDialog)
CImportStanMagazynowyD::CImportStanMagazynowyD()
: CMyDialog(MYIDD(IDD_STANYMAGAZYNOWE_IMPORT), NULL)
{
    setRegSaveDataT(_T("Import\\StanMagazynowy"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_StanyMag, true);
}

CImportStanMagazynowyD::~CImportStanMagazynowyD()
{
    saveRegSaveDataT();
}

void CImportStanMagazynowyD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_TMG_IDMAGAZYNU, m_tmg_idmagazynu);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportStanMagazynowyD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("tmg_idmagazynu", m_tmg_idmagazynu);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportStanMagazynowyD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportStanMagazynowyD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportStanMagazynowyD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportStanMagazynowyD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportStanMagazynowyD_DM);
};


DIALOGMANAGER_REGISTERT("ImportStanyMagazynowe", CImportStanMagazynowyD_DM);


bool CImportStanMagazynowyD_DM::executeForParam(const CMyString& param)
{
    CImportStanMagazynowyD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportStanMagazynowyD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}

void CImportStanMagazynowyD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonStanMagazynowyImport* sm = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) sm = new CExcelStanyMagazynoweImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        sm = new CCSVStanyMagazynoweImport();
        if (sm) ((CCSVStanyMagazynoweImport*)sm)->setCSVConfig(csvconf);
    }

    sm->setIDMagazynu(DTGDTGETPOLET("tmg_idmagazynu"));
    sm->setFileName(DTGDTGETPOLET("_fileName"));
    sm->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    sm->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, sm, 0);

    if (sm->isError())
    {
        AfxMultiMessageBox(sm->getErrorMessage());
        DELETE_WSK(sm);
        return;
    }

    DELETE_WSK(sm);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji"));

    CMyDialog::OnOK();
}

void CImportStanMagazynowyD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonStanMagazynowyImport>& prm)
{
    CCommonStanMagazynowyImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s stanów magazynowych z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


CUInfo CImportStanMagazynowyD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportStanMagazynowyD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_STANYMAGAZYNOWE));
}

INT_PTR CImportStanMagazynowyD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_STANYMAGAZYNOWE))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
