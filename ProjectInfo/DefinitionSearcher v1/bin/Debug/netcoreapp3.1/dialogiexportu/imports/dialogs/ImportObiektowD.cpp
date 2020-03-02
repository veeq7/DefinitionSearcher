#include "StdAfx.h"
#include "managerdll.h"
#include "ImportObiektowD.h"

#include "wti.h"
#include "../excel/ExcelObiektowImport.h"
#include "../CommonObiektowImport.h"
#include "../csv/CSVObiektowImport.h"
#include "dialogs/male/CSVParametryD.h"
//Szczegolowe


// CImportObiektowD dialog

IMPLEMENT_DYNAMIC(CImportObiektowD, CMyDialog)
CImportObiektowD::CImportObiektowD()
: CMyDialog(MYIDD(IDD_OBIEKTOW_IMPORT), NULL)
{
    setRegSaveDataT(_T("Import\\Obiektow"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Urzadzenia, true);
}

CImportObiektowD::~CImportObiektowD()
{
    saveRegSaveDataT();
}

void CImportObiektowD::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    try { DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN); } catch (...) {}
    DDXCTRL(IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXCTRL(IDC_I_CFICSV, m_i_cficsv);
    DDXCTRL(IDC_CSV_PARAMS, m_csv_params);
    DDXCTRL(IDC_C_WARTDOW, m_im_wartoscidowolnych);
    DDXCTRL(IDC_C_LACZOB_PZ, m_im_laczenieobiektow_dokprzyj);
    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportObiektowD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_wartoscidow", m_im_wartoscidowolnych);
    DC_REGISTERT("_laczenieob_numser", m_im_laczenieobiektow_dokprzyj);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportObiektowD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportObiektowD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportObiektowD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}


class CImportObiektowD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportObiektowD_DM);
};


DIALOGMANAGER_REGISTERT("ImportObiektowD", CImportObiektowD_DM);


bool CImportObiektowD_DM::executeForParam(const CMyString& param)
{
    CImportObiektowD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportObiektowD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}


void CImportObiektowD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonObiektowImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelObiektowImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVObiektowImport();
        if (ci) ((CCSVObiektowImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setImportWartosciDowolne(_atoi(DTGDTGETPOLET("_im_wartoscidow")), TG_OBIEKTY);
    ci->setHowActualize(getHowActualize());
    ci->setPowiazanieObiektowZPZ(_atoi(DTGDTGETPOLET("_laczenieob_numser")));
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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" obiektów"));

    CMyDialog::OnOK();
}

void CImportObiektowD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonObiektowImport>& prm)
{
    CCommonObiektowImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s obiektów/urządzeń z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s obiektów/urządzeń."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportObiektowD::getHowActualize()
{
    return ACTUALIZE_UPDATE;
}
// CImportObiektowD message handlers

CUInfo CImportObiektowD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportObiektowD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_OBIEKTOW));
}

INT_PTR CImportObiektowD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_OBIEKTOW))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
