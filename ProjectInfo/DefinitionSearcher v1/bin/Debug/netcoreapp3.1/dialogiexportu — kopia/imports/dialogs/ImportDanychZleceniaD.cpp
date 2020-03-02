#include "StdAfx.h"
#include "managerdll.h"
#include "ImportDanychZleceniaD.h"

#include "docinfo.h"
#include "wti.h"
//DIALOGIEXPORTU
#include "../CommonZlecenieImport.h"
#include "dialogs/male/CSVParametryD.h"
//Szczegolowe

// CImportDanychZleceniaD dialog

IMPLEMENT_DYNAMIC(CImportDanychZleceniaD, CMyDialog)
CImportDanychZleceniaD::CImportDanychZleceniaD()
: CMyDialog(MYIDD(IDD_ZLECENIA_IMPORT), NULL)
{
    setRegSaveDataT(_T("Import\\Zlecenia"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_DaneZlecen, true);
}

CImportDanychZleceniaD::~CImportDanychZleceniaD()
{
    saveRegSaveDataT();
}

void CImportDanychZleceniaD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportDanychZleceniaD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportDanychZleceniaD::UkryjCoNiepotrzebne()
{
    m_fileName.setFilter(getFileMask());

    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}

BEGIN_MESSAGE_MAP(CImportDanychZleceniaD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportDanychZleceniaD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportDanychZleceniaD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportDanychZleceniaD_DM);
};

DIALOGMANAGER_REGISTERT("ImportDaneZleceniaD", CImportDanychZleceniaD_DM);

bool CImportDanychZleceniaD_DM::executeForParam(const CMyString& param)
{
    CImportDanychZleceniaD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportDanychZleceniaD::getFileMask()
{
    CMyString f = DTGDTGETPOLET("_format");

    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}

void CImportDanychZleceniaD::OnOK()
{
    _ileZaimportowano = 0;
    _ilePominieto = 0;

    CCommonZlecenieImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CXLSZlecenieImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVZlecenieImport();
        if (ci) ((CCSVZlecenieImport*)ci)->setCSVConfig(csvconf);
    }

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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji (pominięto ") + CMyString(_ilePominieto) + _T(")"));


    CMyDialog::OnOK();
}

void CImportDanychZleceniaD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonZlecenieImport>& prm)
{
    CCommonZlecenieImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s danych zleceń z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute();
        _ilePominieto += eti->ilePominieto();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s, pominęto %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano), CMyString(_ilePominieto)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


CUInfo CImportDanychZleceniaD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportDanychZleceniaD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_DANEZLECENIA));
}

INT_PTR CImportDanychZleceniaD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_DANEZLECENIA))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
