#include "StdAfx.h"
#include "managerdll.h"
#include "ImportTowaryD.h"
#include "../csv/CSVTowarImport.h"
#include "../excel/exceltowarimport.h"
#include "../xml/xmltowarimport.h"
#include "../commontowarimport.h"
#include "../TowarImportDecisionHelper.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
#include "apps/procad/ProcadTowaryImport.h"
//Szczegolowe
#include "../doclogic/managerex/keyscache/ImportKeysCache.h"

// CImportTowaryD dialog

IMPLEMENT_DYNAMIC(CImportTowaryD, CMyDialog)
CImportTowaryD::CImportTowaryD(bool isCennikImportMode)
    : CMyDialog(MYIDD(isCennikImportMode ? IDD_FULLCENNIK_IMPORT : IDD_TOWARY_IMPORT), NULL)
    , _cennikImportMode(isCennikImportMode)
{
    setRegSaveDataT(_cennikImportMode ? _T("Import\\FullCennik") : _T("Import\\Towary"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cfixml, _T("CFIXML"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_i_cfiexcel.addRadio(&m_i_procadxml, _T("PROCADXML"));

    m_ex_pomin.addRadio(&m_ex_pomin, HOWACT_POMIN);
    m_ex_pomin.addRadio(&m_ex_actualize, HOWACT_AKTUALIZUJ);
    m_ex_pomin.addRadio(&m_ex_onlyceny, HOWACT_ONLYCENY);
    m_ex_pomin.addRadio(&m_ex_tylkoaktualizacja, HOWACT_TYLKOAKTUALIZUJ);
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_onexists"), HOWACT_POMIN);

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Towar, true);
}

CImportTowaryD::~CImportTowaryD()
{
    saveRegSaveDataT();
}

void CImportTowaryD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    try { DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN); }
    catch (...) {}
    DDXCTRL(IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXCTRL(IDC_I_PROCADXML, m_i_procadxml);
    DDXCTRL(IDC_EX_POMIN, m_ex_pomin);
    DDXCTRL(IDC_EX_ACTUALIZE, m_ex_actualize);
    DDXCTRL(IDC_EX_CENNIK, m_ex_onlyceny);
    DDXCTRL(IDC_EX_TYLKOAKTUALIZACJA, m_ex_tylkoaktualizacja);
    DDXCTRL(IDC_I_CFIXMLEM, m_i_cfixml);
    DDXCTRL(IDC_I_CFICSV, m_i_cficsv);
    DDXCTRL(IDC_CSV_PARAMS, m_csv_params);
    DDXCTRL(IDC_C_WARTDOW, m_im_wartoscidowolnych);
    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportTowaryD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_onexists", m_ex_pomin);
    DC_REGISTERT("_im_wartoscidow", m_im_wartoscidowolnych);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportTowaryD::UkryjCoNiepotrzebne()
{
    m_fileName.setFilter(getFileMask());

    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    _ENABLE_ITO(m_ex_onlyceny);
    _ENABLE_ITO(m_ex_tylkoaktualizacja);

    if (GET_POLE(_T("_format")) == _T("PROCADXML"))
    {
        if ((GET_POLE(_T("_onexists")) != CMyString(HOWACT_AKTUALIZUJ))
            && (GET_POLE(_T("_onexists")) != CMyString(HOWACT_POMIN)))
            getDataT()->setPole(_T("_onexists"), CMyString(HOWACT_POMIN));

        _DISABLE_ITO(m_ex_onlyceny);
        _DISABLE_ITO(m_ex_tylkoaktualizacja);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportTowaryD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportTowaryD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}


class CImportTowaryD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATEFUNC(importTowary, CImportTowaryD_DM);
    DIALOGMANAGER_ITEMCREATEFUNC(importCennikMulti, CImportTowaryD_DM);
};


DIALOGMANAGER_REGISTERITEMT("ImportCennikMultiD", CImportTowaryD_DM, importCennikMulti);
DIALOGMANAGER_REGISTERITEMT("ImportTowaryD", CImportTowaryD_DM, importTowary);


bool CImportTowaryD_DM::executeForParam(const CMyString& param)
{
    CImportTowaryD dlg(EQSTRINGT(getName(), "ImportCennikMultiD"));
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportTowaryD::getFileMask()
{
    CMyString f = DTGDTGETPOLET("_format");

    if (EQSTRINGT(f, "CFIEXCEL"))
        return _FT("%sPliki XML (*.xml)|*.xml|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);

    if (EQSTRINGT(f, "CFIXML") || EQSTRINGT(f, "PROCADXML"))
        return _FT("Pliki XML (*.xml)|*.xml|%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);

    if (EQSTRINGT(f, "CFICSV"))
        return _FT("Pliki CSV (*.csv)|*.csv|%sPliki XML (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);

    return _FT("%sPliki XML (*.xml)|*.xml|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);
}


CCommonImportDataProvider* CImportTowaryD::getDataProvider()
{
    CMyString f = DTGDTGETPOLET("_format");

    //Format XLS 
    if (EQSTRINGT(f, "CFIEXCEL"))
    {
        CExcelTowarImport* ti = new CExcelTowarImport(_cennikImportMode, getHowActualize());
        if (
            !ti->openFile(DTGDTGETPOLET("_fileName"))
            )
        {
            AfxMessageBox(ti->getErrorMessage());
            delete ti;
            return NULL;
        }

        return ti;
    }
    else if (EQSTRINGT(f, "CFIXML"))
        //Format XML
    {

        CXMLTowarImport* ti = new CXMLTowarImport(_cennikImportMode, getHowActualize());
        if (
            !ti->openFile(DTGDTGETPOLET("_fileName"))
            )
        {
            AfxMessageBox(ti->getErrorMessage());
            delete ti;
            return NULL;
        }

        return ti;
    }
    else if (EQSTRINGT(f, "CFICSV"))
        //Format CSV
    {
        CCSVTowarImport* ci = new CCSVTowarImport(_cennikImportMode, getHowActualize());
        ci->setCSVConfig(csvconf);
        if (!ci->openFile(DTGDTGETPOLET("_fileName")))
        {
            AfxMessageBox(ci->getErrorMessage());
            delete ci;
            return NULL;
        }

        return ci;
    }

    return NULL;
}

void CImportTowaryD::OnOK()
{
    _ileZaimportowano = 0;

    CMyString f = DTGDTGETPOLET("_format");

    //Format XLS 
    if (EQSTRINGT(f, "PROCADXML"))
        return onProcadImport();

    CCommonImportDataProvider* dp = getDataProvider();
    if (!dp) return;

    //Utworz dostarczyciela danych
    CImportDataProviderC dataProvider(dp);

    CCommonWTIDlgTPL<CImportDataProviderC>* dlg(this);
    WTI_NDLG(*dlg, &dataProvider, 0);

    //Sprawdz bledy z data providera
    if (dataProvider->isError())
    {
        AfxMultiMessageBox(dataProvider->getErrorMessage());
        return;
    }

    AfxMessageBox(_TX("Dodano ") + CMyString(dataProvider->getIleDodano()) + _T(",zaktualizowano ") + CMyString(dataProvider->getIleZaktualizowano()) + _T(" kartotek"));

    CMyDialog::OnOK();
}

void CImportTowaryD::onProcadImport()
{
    CProcadTowaryImport* procadTI = new CProcadTowaryImport();

    if (!procadTI)
        return;

    procadTI->setHowActualize(getHowActualize());
    procadTI->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    procadTI->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, procadTI, 1);

    if (procadTI->isError())
    {
        AfxMultiMessageBox(procadTI->getErrorMessage());
        delete procadTI;
        return;
    }

    delete procadTI;

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _TX(" rekordów"));

    CMyDialog::OnOK();
}

void CImportTowaryD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CProcadTowaryImport>& prm)
{
    if (prm.getType() == 1)
    {
        CProcadTowaryImport* procadTI = prm.getParam();
        if (procadTI)
        {
            procadTI->setStatusRetHandler(sr);
            CDataT::toLog(_FX(_TX("Rozpoczęto import %s towarów z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            CCommonImportTIDImportParams prms;
            _ileZaimportowano = procadTI->importFile(DTGDTGETPOLET("_fileName"), prms);
            CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            procadTI->clearStatusRetHandler();
        }
    }
}

void CImportTowaryD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CImportDataProviderC>& prm)
{
    CImportDataProviderC* dp = prm.getParam();
    CImportKeysCacheC keysCache(new CImportKeysCache());

    int linia = 0;
    int dobrych = 0;
    int pominieto = 0;
    CSArray bledy;

    bool osobneTransakcje = DTGDTGETPOLEINTT("_im_ostrans");

    CClassTransactionEx ct;
    if (!osobneTransakcje) ct.BeginTransaction();

    if (dp)
    {
        CDataT::toLog(_FX(_TX("Rozpoczęto import towarów %s z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        while (!(*dp)->isEOF() && !sr->isAborded())
        {
            if (osobneTransakcje) ct.BeginTransaction();
            linia++;

            {
                CCommonTowarImport cti(_cennikImportMode);
                cti.setImportWartosciDowolne(DTGDTGETPOLEINTT("_im_wartoscidow"), TG_TOWARY);
                cti.setNrLinii(linia);
                cti.setHowActualize(getHowActualize());
                cti.setStatusRetHandler(sr);

                cti.setKeysCache(keysCache);
                cti.setDataProvider(*dp);
                cti.setDecisionsHelper(new CTowarImportDecisionHelper());

                bool wynik = cti.performOperationEx();
                cti.clearStatusRetHandler();

                if (!DTGDTGETPOLEINTT("_im_bladprzerywa"))
                {
                    if (wynik)
                    {
                        if (osobneTransakcje)
                            ct.CloseTransaction();

                        dobrych++;
                    }
                    if (!wynik)
                    {
                        if (osobneTransakcje)
                            ct.CloseTransaction(false);

                        if (cti.isError())
                            bledy.Add(cti.getBledy());
                        else
                            pominieto++;
                    }
                }

                if (cti.isError() && DTGDTGETPOLEINTT("_im_bladprzerywa"))
                {
                    (*dp)->setErrorMessageExt(cti.getErrorMessage());
                    break;
                }
            }
            (*dp)->moveNext();
        }

        if (bledy.GetCount())
        {
            (*dp)->setErrorMessageExt(CCommonImport::getKomunikatKoncowy(bledy.GetCount(), dobrych, pominieto));
            CCommonImport::zapiszBledyS(bledy.Splaszcz(PRPNSTRING));
        }

        CDataT::toLog(_FX(_TX("Zakończono import towarów %s z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
    }

    if (!osobneTransakcje) ct.CloseTransaction();
}


eHowActualize CImportTowaryD::getHowActualize()
{
    return (eHowActualize)_atoi(DTGDTGETPOLET("_onexists"));
}


CUInfo CImportTowaryD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportTowaryD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_TOWARY));
}

INT_PTR CImportTowaryD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_TOWARY))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}

BOOL CImportTowaryD::OnInitDialog()
{
    BOOL ret = __super::OnInitDialog();


    return ret;
}
