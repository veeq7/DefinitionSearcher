#include "StdAfx.h"
// #include "managerdll.h"
#include "ImportKatalogPrzynaleznosciD.h"

//
////dialogiexportu
#include "dialogs/male/CSVParametryD.h"
#include "../interfacesgui/modules/import/ImportInFiltr.h"
#include "imports/dialogs/ImportWartosciDowD.h"
#include "../CommonKatalogPrzynaleznoscImport.h"
#include "../excel/ExcelKatalogPrzynaleznoscImport.h"
#include "../xml/XMLKatalogPrzynaleznoscImport.h"
#include "../csv/CSVKatalogPrzynaleznoscImport.h"
#include "wti.h"

// CImportKatalogPrzynaleznosciD dialog

IMPLEMENT_DYNAMIC(CImportKatalogPrzynaleznosciD, CMyDialog)
CImportKatalogPrzynaleznosciD::CImportKatalogPrzynaleznosciD()
: CMyDialog(CImportKatalogPrzynaleznosciD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\Przynaleznosci"));

    m_map_kod.addKeysValues(GetSearchMap(TG_TOWARY));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cfixml, _T("CFIXML"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));

    m_noex_pomin.addRadio(&m_noex_pomin, _T("NOEXPOMIN"));
    m_noex_pomin.addRadio(&m_noex_utworz, _T("NOEXUTWORZ"));
    m_noex_pomin.addRadio(&m_noex_blad, _T("NOEXBLAD"));

    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_noexist"), _T("NOEXBLAD"));
    setDefVal(getDataT(), _T("_mapKod"), CMyString(STOWAR_BY_KOD));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_PrzynaleznosciKatalogu, true);
}

CImportKatalogPrzynaleznosciD::~CImportKatalogPrzynaleznosciD()
{
    saveRegSaveDataT();
}

void CImportKatalogPrzynaleznosciD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    try {
        DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    } catch (...) {}
    DDXCTRL(IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXCTRL(IDC_I_CFIXMLEM, m_i_cfixml);
    DDXCTRL(IDC_I_CFICSV, m_i_cficsv);
    DDXCTRL(IDC_CSV_PARAMS, m_csv_params);
    DDXCTRL(IDC_NOEXIST_POMIN, m_noex_pomin);
    DDXCTRL(IDC_NOEXIST_UTWORZ, m_noex_utworz);
    DDXCTRL(IDC_NOEXIST_BLAD, m_noex_blad);
    DDXCTRL(IDC_MAP_KOD, m_map_kod);
    DDXCTRL(IDOK, m_ok);
    DDXCTRL(IDCANCEL, m_cancel);
    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
    __super::DoDataExchange(pDX);
}

bool CImportKatalogPrzynaleznosciD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_noexist", m_noex_pomin);
    DC_REGISTERT("_mapKod", m_map_kod);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportKatalogPrzynaleznosciD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportKatalogPrzynaleznosciD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportKatalogPrzynaleznosciD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

CMyString CImportKatalogPrzynaleznosciD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki XML (*.xml)|*.xml|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");
}

CCommonImportDataProvider* CImportKatalogPrzynaleznosciD::getDataProvider()
{
    CMyString f = DTGDTGETPOLET("_format");

    //Format XLS 
    if (EQSTRINGT(f, "CFIEXCEL"))
    {
        CExcelKatalogPrzynaleznoscImport* ti = new CExcelKatalogPrzynaleznoscImport();
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

        CXMLKatalogPrzynaleznoscImport* ti = new CXMLKatalogPrzynaleznoscImport();
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
        CCSVKatalogPrzynaleznoscImport* ci = new CCSVKatalogPrzynaleznoscImport();
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


void CImportKatalogPrzynaleznosciD::OnOK()
{
    CMyString f = DTGDTGETPOLET("_format");

    CCommonImportDataProvider* dp = getDataProvider();
    if (!dp) return;

    //Utworz dostarczyciela danych
    CImportDataProviderC dataProvider(dp);

    WTI_NDLG(*this, &dataProvider, 0);

    //Sprawdz bledy z data providera
    if (dataProvider->isError())
    {
        AfxMultiMessageBox(dataProvider->getErrorMessage());
        return;
    }

    {
        CMyString msg;
        msg = _TX("Zaktualizowano ") + CMyString(dataProvider->getIleZaktualizowano()) + _TX(" kartotek.");
        if (dataProvider->getIleDodano() != 0)
            msg += _TX(" Dodano ") + CMyString(dataProvider->getIleDodano()) + _TX(" pozycji katalogu.");
        AfxMessageBox(msg);
    }

    CMyDialog::OnOK();
}

void CImportKatalogPrzynaleznosciD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CImportDataProviderC>& prm)
{
    CImportDataProviderC* dp = prm.getParam();
    CImportKeysCacheC keysCache(new CImportKeysCache());

    int linia = 0;
    int dobrych = 0;
    int pominieto = 0;
    CSArray bledy;
    CTreeCatalogHelper tchelper;

    bool osobneTransakcje = DTGDTGETPOLEINTT("_im_ostrans");

    CClassTransactionEx ct;
    if (!osobneTransakcje) ct.BeginTransaction();

    if (dp)
    {
        CDataT::toLog(_FX(_TX("Rozpoczęto import przynależności do katalogu z pliku %s."), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        while (!(*dp)->isEOF() && !sr->isAborded())
        {
            if (osobneTransakcje) ct.BeginTransaction();
            linia++;

            {
                CCommonKatalogPrzynaleznoscImport cti;
                cti.setStatusRetHandler(sr);
                cti.setKeysCache(keysCache);
                cti.setDataProvider(*dp);
                cti.setMapowanieKodu((eTowarSearchBy)_atoi(DTGDTGETPOLET("_mapKod")));
                if (DTGDTGETPOLET("_noexist") == _T("NOEXPOMIN"))
                    cti.setOnNieistniejacyKatalog(KATALOG_NOEXIST_SKIP);
                else
                    if (DTGDTGETPOLET("_noexist") == _T("NOEXUTWORZ"))
                        cti.setOnNieistniejacyKatalog(KATALOG_NOEXIST_CREATE);
                    else
                        cti.setOnNieistniejacyKatalog(KATALOG_NOEXIST_ERROR);
                cti.setTCHelper(&tchelper);

                bool wynik = cti.performOperationEx();

                cti.clearStatusRetHandler();

                if (wynik)
                {
                    if (osobneTransakcje)
                        ct.CloseTransaction();

                    dobrych++;
                }

                if (!DTGDTGETPOLEINTT("_im_bladprzerywa"))
                {
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

        CDataT::toLog(_FX(_TX("Zakończono import przynależności do katalogu z pliku %s."), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
    }
}


int CImportKatalogPrzynaleznosciD::getHowActualize()
{
    return ACTUALIZE_UPDATE;
}

/*
class CTestThread : public CMyThread
{
DECLARE_DYNCREATE(CTestThread)
public:
CTestThread() {}

INT_PTR Run();

bool onWantEndThread();

int no;
};

int threadNos(0);

INT_PTR CTestThread::Run()
{
signalBeginThread();

no=++threadNos;

srand (time(NULL));

while (mayRun())
{
CClassTransaction ct;
ct.BeginTransaction();

Sleep(abs(rand())%10);

ct.execQuery(_FT("UPDATE abcd SET cnt=cnt+1,n=%s WHERE d=%s",CMyString(no),CMyString(abs(rand())%3000)));

Sleep(abs(rand())%20);

ct.CloseTransaction();

Sleep(abs(rand())%10);
}


return onCommonRunEnd();
}

bool CTestThread::onWantEndThread()
{
return __super::onWantEndThread();
}

IMPLEMENT_DYNCREATE(CTestThread,CMyThread)

*/

INT_PTR CImportKatalogPrzynaleznosciD::DoModal()
{
    /*
    for (int i=0;i<20;i++)
    {
    CTestThread* th=DYNAMIC_DOWNCAST(CTestThread,NEWTHREADT(AfxBeginThread(RUNTIME_CLASS(CTestThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED),"TestThread"));

    CMyThreadPool::getPool()->AddThread(th);

    th->ResumeThread();
    th->InternalRelease();
    }

    */
    if (!CanDoIt(UPR_IMPORT_TOWARY))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }

    return __super::DoModal();
}

CUInfo CImportKatalogPrzynaleznosciD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }

    return __super::getPermission(no - 1);
}

CUInfo CImportKatalogPrzynaleznosciD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_TOWARY));
}

/////////////////////////////////////////////////////////////////
//////
/////////////////////////////////////////////////////////////////
class CImportKatalogPrzynaleznosciD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATEFUNC(importKatalogPrzynaleznosci, CImportKatalogPrzynaleznosciD_DM);
};


DIALOGMANAGER_REGISTERITEMT("ImportKatalogPrzynaleznosciD", CImportKatalogPrzynaleznosciD_DM, importKatalogPrzynaleznosci);


bool CImportKatalogPrzynaleznosciD_DM::executeForParam(const CMyString& param)
{
    CImportKatalogPrzynaleznosciD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}
