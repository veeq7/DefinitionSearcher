#include "StdAfx.h"
#include "managerdll.h"
#include "ImportCzZamiennychD.h"

#include "../excel/ExcelCzZamienneImport.h"
#include "../CommonCzZamiennaImport.h"
#include "../csv/CSVCzZamienneImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
//Szczegolowe

// CImportCzZamiennychD dialog

IMPLEMENT_DYNAMIC(CImportCzZamiennychD, CMyDialog)
CImportCzZamiennychD::CImportCzZamiennychD()
: CMyDialog(CImportCzZamiennychD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\CzZamienne"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_CzesciZamienne, true);
}

CImportCzZamiennychD::~CImportCzZamiennychD()
{
    saveRegSaveDataT();
}

void CImportCzZamiennychD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportCzZamiennychD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportCzZamiennychD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportCzZamiennychD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportCzZamiennychD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportCzZamiennychD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportCzZamiennychD_DM);
};

DIALOGMANAGER_REGISTERT("ImportCzZamiennychD", CImportCzZamiennychD_DM);

bool CImportCzZamiennychD_DM::executeForParam(const CMyString& param)
{
    CImportCzZamiennychD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportCzZamiennychD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki MS-Excel 2007 (*.xlsx)|*.xlsx|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");
}

void CImportCzZamiennychD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonCzZamiennaImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL"))
        ci = new CExcelCzZamienneImport();

    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVCzZamienneImport();
        if (ci) ((CCSVCzZamienneImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setHowActualize(getHowActualize());
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        delete ci;
        return;
    }

    delete ci;

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _TX(" rekordów"));

    CMyDialog::OnOK();
}

void CImportCzZamiennychD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonCzZamiennaImport>& prm)
{

    CCommonCzZamiennaImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s części zamiennch z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportCzZamiennychD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}
// CImportCzZamiennychD message handlers

// Upr
CUInfo CImportCzZamiennychD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportCzZamiennychD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_CZZAMIENNE));
}

INT_PTR CImportCzZamiennychD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_CZZAMIENNE))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}