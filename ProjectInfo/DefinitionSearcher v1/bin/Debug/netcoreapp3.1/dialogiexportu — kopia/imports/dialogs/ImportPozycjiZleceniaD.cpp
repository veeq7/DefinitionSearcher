#include "StdAfx.h"
#include "managerdll.h"
#include "ImportPozycjiZleceniaD.h"

#include "docinfo.h"
#include "wti.h"
//DIALOGIEXPORTU
#include "../CommonPozycjaZleceniaImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "../managerdll/manager/PlanZleceniaT.h"
//Szczegolowe

// CImportPozycjiZleceniaD dialog

IMPLEMENT_DYNAMIC(CImportPozycjiZleceniaD, CMyDialog)
CImportPozycjiZleceniaD::CImportPozycjiZleceniaD(const CDTTypeID& tid)
    : CMyDialog(MYIDD(IDD_POZYCJE_ZLECENIA_IMPORT), NULL)
{
    if (tid.getType() == TG_PLANZLECENIA)
    {
        this->pz_idplanu = tid.getID();
        this->zl_idzlecenia = CPlanZleceniaT::getIDZleceniaS(pz_idplanu);
    }

    if (tid.getType() == TG_ZLECENIA) this->zl_idzlecenia = tid.getID();    

    setRegSaveDataT(_T("Import\\PozycjeZlecenia\\") + tid.getTypeAsString());
    dataType = tid.getType();

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    m_przelicznik_pozycji.setDokladnosc(formatowanieIlosci);
    m_r_aktualizuj.addRadio(&m_r_aktualizuj, _T("A"));
    m_r_aktualizuj.addRadio(&m_r_pomin, _T("P"));

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_przelicznik_pozycji"), ONESTRING);
    setDefVal(getDataT(), _T("_r_aktualizuj"), _T("A"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_PozycjeZlecenia, true);
}

CImportPozycjiZleceniaD::~CImportPozycjiZleceniaD()
{
    saveRegSaveDataT();
}

void CImportPozycjiZleceniaD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDXExchange(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXExchange(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDXExchange(pDX, IDC_CSV_PARAMS, m_csv_params);

    DDXExchange(pDX, IDC_PRZELICZNIK_POZYCJI, m_przelicznik_pozycji);
    DDXExchange(pDX, IDC_C_ANULOWANIE_NIEZNALEZIONYCH, m_c_anulowanie_nieznalezionych);
    DDXExchange(pDX, IDC_R_AKTUALIZUJ, m_r_aktualizuj);
    DDXExchange(pDX, IDC_R_POMIN, m_r_pomin);
    DDXExchange(pDX, IDC_C_NIEUWZGLEDNIAJ_NOWYCH, m_im_nieUwzglNowych);

    DDXExchange(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDXExchange(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportPozycjiZleceniaD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);

    DC_REGISTERT("_przelicznik_pozycji", m_przelicznik_pozycji);
    DC_REGISTERT("_c_anulowanie_nieznalezionych", m_c_anulowanie_nieznalezionych);
    DC_REGISTERT("_r_aktualizuj", m_r_aktualizuj);

    DC_REGISTERT("_im_nieUwzglNowych", m_im_nieUwzglNowych);

    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportPozycjiZleceniaD::UkryjCoNiepotrzebne()
{
    m_fileName.setFilter(getFileMask());

    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    _ENDIS_IT(m_przelicznik_pozycji, (dataType == TG_PLANZLECENIA));

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}

BEGIN_MESSAGE_MAP(CImportPozycjiZleceniaD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportPozycjiZleceniaD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportPozycjiZleceniaD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportPozycjiZleceniaD_DM);
};

DIALOGMANAGER_REGISTERT("ImportPozycjeZleceniaD", CImportPozycjiZleceniaD_DM);

bool CImportPozycjiZleceniaD_DM::executeForParam(const CMyString& param)
{
    CDTTypeID tid; tid.getFromString(param);
    
    if (!tid.isOK())
        return true;

    CImportPozycjiZleceniaD dlg(tid);
    dlg.DoModal();

    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportPozycjiZleceniaD::getFileMask()
{
    CMyString f = DTGDTGETPOLET("_format");

    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);
}

void CImportPozycjiZleceniaD::OnOK()
{
    _ileZaimportowano = 0;
    _ilePominieto = 0;

    CCommonPozycjaZleceniaImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CXLSPozycjaZleceniaImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVPozycjaZleceniaImport();
        if (ci) ((CCSVPozycjaZleceniaImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setZlecenieID(zl_idzlecenia);
    ci->setPozycjaZleceniaID(pz_idplanu);

    ci->setPrzelicznikPozycji((DTGDTGETPOLET("_przelicznik_pozycji")));
    ci->setAnulowanieNieznalezionych(DTGDTGETPOLEBOOLT("_c_anulowanie_nieznalezionych"));
    ci->setAktualizacja(DTGDTGETPOLET("_r_aktualizuj") == _T("A"));
    ci->setNieUwzglNowych(DTGDTGETPOLEBOOLT("_im_nieUwzglNowych"));

    ci->setFileName(DTGDTGETPOLET("_fileName"));
    ci->setOsobneTransakcje(DTGDTGETPOLEBOOLT("_im_ostrans"));
    ci->setBladPrzerywa(DTGDTGETPOLEBOOLT("_im_bladprzerywa"));

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

void CImportPozycjiZleceniaD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonPozycjaZleceniaImport>& prm)
{
    CCommonPozycjaZleceniaImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s pozycji zlecenia z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->performBeforeExecute();
        _ileZaimportowano = eti->execute();
        _ilePominieto += eti->ilePominieto();
        eti->performAfterExecute();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s, pominęto %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano), CMyString(_ilePominieto)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}

CUInfo CImportPozycjiZleceniaD::getPermission(int no)
{
    switch (no)
    {
        case 0: return getMainPermission();
    }

    return __super::getPermission(no - 1);
}

CUInfo CImportPozycjiZleceniaD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_DANEZLECENIA));
}

INT_PTR CImportPozycjiZleceniaD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_DANEZLECENIA))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }

    return __super::DoModal();
}
