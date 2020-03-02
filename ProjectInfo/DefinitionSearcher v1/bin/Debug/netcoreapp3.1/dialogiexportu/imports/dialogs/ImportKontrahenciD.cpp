#include "StdAfx.h"
#include "managerdll.h"
#include "ImportKontrahenciD.h"

#include "wti.h"

//Szczegolowe
#include "../excel/excelklientimport.h"
#include "dialogs/male/CSVParametryD.h"
#include "../csv/CSVKlientImport.h"

// CImportKontrahenciD dialog

IMPLEMENT_DYNAMIC(CImportKontrahenciD, CMyDialog)
CImportKontrahenciD::CImportKontrahenciD()
: CMyDialog(CImportKontrahenciD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\Kontrahenci"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_ex_pomin.addRadio(&m_ex_pomin, _T("SKIP"));
    m_ex_pomin.addRadio(&m_ex_actualize, _T("UPDATE"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_onexists"), _T("SKIP"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Klient, true);
}

CImportKontrahenciD::~CImportKontrahenciD()
{
    saveRegSaveDataT();
}

void CImportKontrahenciD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_EX_POMIN, m_ex_pomin);
    DDX_Control(pDX, IDC_EX_ACTUALIZE, m_ex_actualize);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_WARTDOW, m_im_wartoscidowolnych);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportKontrahenciD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_onexists", m_ex_pomin);
    DC_REGISTERT("_im_wartoscidow", m_im_wartoscidowolnych);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportKontrahenciD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}



BEGIN_MESSAGE_MAP(CImportKontrahenciD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportKontrahenciD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportKontrahenciD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportKontrahenciD_DM);
};


DIALOGMANAGER_REGISTERT("ImportKontrahenciD", CImportKontrahenciD_DM);


bool CImportKontrahenciD_DM::executeForParam(const CMyString& param)
{
    CImportKontrahenciD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CImportKontrahenciD_DM* ikd = new CImportKontrahenciD_DM();


CMyString CImportKontrahenciD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}


void CImportKontrahenciD::OnOK()
{
    _ileZaimportowano = 0;

    CImportKeysCacheC kc(new CImportKeysCache());

    CCommonKlientImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelKlientImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVKlientImport();
        if (ci) ((CCSVKlientImport*)ci)->setCSVConfig(csvconf);
    }

    if (!ci) return;
    ci->setImportWartosciDowolne(_atoi(DTGDTGETPOLET("_im_wartoscidow")), TB_KLIENT);
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));
    ci->setHowActualize(getHowActualize());
    ci->setKeysCache(kc);

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        DELETE_WSK(ci);
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" kartotek"));

    CMyDialog::OnOK();
}

void CImportKontrahenciD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonKlientImport>& prm)
{
    CCommonKlientImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s kontrahentów z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s kontrahentów."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportKontrahenciD::getHowActualize()
{
    if (DTGDTGETPOLET("_onexists") == _T("UPDATE")) return ACTUALIZE_UPDATE;
    return ACTUALIZE_SKIP;
}

CUInfo CImportKontrahenciD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportKontrahenciD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_KLIENCI));
}

INT_PTR CImportKontrahenciD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_KLIENCI))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}

