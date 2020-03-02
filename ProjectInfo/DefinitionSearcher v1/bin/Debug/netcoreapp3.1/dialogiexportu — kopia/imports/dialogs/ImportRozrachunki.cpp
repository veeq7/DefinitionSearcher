#include "StdAfx.h"
#include "managerdll.h"
#include "ImportRozrachunki.h"

#include "docinfo.h"
#include "kontrolki.h"
#include "wti.h"
#include "../excel/excelrozrachunkiimport.h"
#include "../CommonRozrachunkiImport.h"
#include "../csv/CSVRozrachunkiImport.h"
#include "dialogs/male/CSVParametryD.h"
//Szczegolowe



// CImportRozrachunkiD dialog

IMPLEMENT_DYNAMIC(CImportRozrachunkiD, CMyDialog)
CImportRozrachunkiD::CImportRozrachunkiD()
: CMyDialog(CImportRozrachunkiD::IDD, NULL), m_tmg_idmagazynu(false), m_tr_seria(false)
{
    setRegSaveDataT(_T("Import\\Rozrachunki"));

    CMyString tr_rok = dzis().Mid(2, 2);
    while (tr_rok.GetLength() < 2) tr_rok = ZEROSTRING + tr_rok;
    setDefVal(getDataT(), _T("tr_rok"), tr_rok);

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Rozrachunki, true);
}

CImportRozrachunkiD::~CImportRozrachunkiD()
{
    saveRegSaveDataT();
}

void CImportRozrachunkiD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXCTRL(IDC_TMG_IDMAGAZYNU, m_tmg_idmagazynu);
    DDXCTRL(IDC_TR_SERIA, m_tr_seria);
    DDXCTRL(IDC_TR_ROK, m_tr_rok);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportRozrachunkiD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("tmg_idmagazynu", m_tmg_idmagazynu);
    DC_REGISTERT("tr_seria", m_tr_seria);
    DC_REGISTERT("tr_rok", m_tr_rok);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);

    return __super::RegisterOnInit();
}

void CImportRozrachunkiD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportRozrachunkiD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportRozrachunkiD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}


class CImportRozrachunkiD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportRozrachunkiD_DM);
};


DIALOGMANAGER_REGISTERT("ImportRozrachunkiD", CImportRozrachunkiD_DM);


bool CImportRozrachunkiD_DM::executeForParam(const CMyString& param)
{
    CImportRozrachunkiD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportRozrachunkiD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}


void CImportRozrachunkiD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonRozrachunkiImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelRozrachunkiImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVRozrachunkiImport();
        if (ci) ((CCSVRozrachunkiImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setMagazyn(DTGDTGETPOLET("tmg_idmagazynu"));
    ci->setParametryNumerow(DTGDTGETPOLET("tr_seria"), DTGDTGETPOLET("tr_rok"), TONSTRING(EMPTYSTRING));
    ci->setFileName(DTGDTGETPOLET("_fileName"));
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        DELETE_WSK(ci);
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji"));

    CMyDialog::OnOK();
}

void CImportRozrachunkiD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonRozrachunkiImport>& prm)
{
    CCommonRozrachunkiImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s rozrachunków z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


CUInfo CImportRozrachunkiD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportRozrachunkiD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_ROZRACHUNKI));
}

INT_PTR CImportRozrachunkiD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_ROZRACHUNKI))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
