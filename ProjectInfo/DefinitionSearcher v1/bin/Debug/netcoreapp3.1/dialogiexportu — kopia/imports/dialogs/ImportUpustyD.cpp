#include "StdAfx.h"
#include "managerdll.h"
#include "ImportUpustyD.h"

#include "../csv/CSVUpustImport.h"
#include "../excel/excelupustyimport.h"
#include "../commonupustimport.h"
#include "dialogs/male/CSVParametryD.h"
#include "../interfacesgui/modules/import/ImportInFiltr.h"
#include "wti.h"
//Szczegolowe
#include "../doclogic/managerex/keyscache/ImportKeysCache.h"

#define IUPUST_INONETRANS       


IMPLEMENT_DYNAMIC(CImportUpustyD, CMyDialog)
CImportUpustyD::CImportUpustyD()
: CMyDialog(CImportUpustyD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\Upusty"));

    m_map_kod.addKeysValues(GetSearchMap(TG_TOWARY));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cfixml, _T("CFIXML"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_ex_pomin.addRadio(&m_ex_pomin, HOWACT_POMIN);
    m_ex_pomin.addRadio(&m_ex_actualize, HOWACT_AKTUALIZUJ);
    m_fileName.setFilter(getFileMask());

    m_ci_sprzedazy.addRadio(&m_ci_sprzedazy, ZEROSTRING);
    m_ci_sprzedazy.addRadio(&m_ci_zakupu, ONESTRING);

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_onexists"), HOWACT_POMIN);
    setDefVal(getDataT(), _T("rodzaj_cen"), ZEROSTRING);
    setDefVal(getDataT(), _T("_mapKod"), CMyString(STOWAR_BY_KOD));


    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_CenaInd, true);
}

CImportUpustyD::~CImportUpustyD()
{
    saveRegSaveDataT();
}

void CImportUpustyD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_EX_POMIN, m_ex_pomin);
    DDX_Control(pDX, IDC_EX_ACTUALIZE, m_ex_actualize);
    DDX_Control(pDX, IDC_I_CFIXMLEM, m_i_cfixml);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_CI_SPRZED, m_ci_sprzedazy);
    DDX_Control(pDX, IDC_CI_ZAKUPU, m_ci_zakupu);
    DDX_Control(pDX, IDC_MAP_KOD, m_map_kod);
    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportUpustyD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_onexists", m_ex_pomin);
    DC_REGISTERT("rodzaj_cen", m_ci_sprzedazy);
    DC_REGISTERT("_mapKod", m_map_kod);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportUpustyD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportUpustyD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportUpustyD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportUpustyD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportUpustyD_DM);
};


DIALOGMANAGER_REGISTERT("ImportUpustyD", CImportUpustyD_DM);


bool CImportUpustyD_DM::executeForParam(const CMyString& param)
{
    CImportUpustyD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportUpustyD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki XML (*.xml)|*.xml|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");
}


CCommonImportDataProvider* CImportUpustyD::getDataProvider()
{
    CMyString f = DTGDTGETPOLET("_format");

    //Format XLS 
    if (EQSTRINGT(f, "CFIEXCEL"))
    {
        CExcelUpustyImport* ti = new CExcelUpustyImport();
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
        CCSVUpustImport* ci = new CCSVUpustImport();
        ci->setCSVConfig(csvconf);
        if (!ci->openFile(DTGDTGETPOLET("_fileName")))
        {
            AfxMessageBox(ci->getErrorMessage());
            delete ci;
            return NULL;
        }

        return ci;
    } /*else if (EQSTRINGT(f,"CFIXML"))
 //Format XML
 {

 CXMLTowarImport* ti=new CXMLTowarImport();
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
 */


    return NULL;
}

void CImportUpustyD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonImportDataProvider* dp = getDataProvider();
    if (!dp) return;

#ifdef IUPUST_INONETRANS
    CClassTransaction ct;
    ct.BeginTransaction();
#endif

    //Utworz dostarczyciela danych
    CImportDataProviderC dataProvider(dp);

    //Wykonaj watek
    WTI_NDLG(*this, &dataProvider, 0);

#ifdef IUPUST_INONETRANS
    ct.CloseTransaction();
#endif

    //Sprawdz bledy z data providera
    if (dataProvider->isError())
    {
        AfxMultiMessageBox(dataProvider->getErrorMessage());
        return;
    }

    AfxMessageBox(_TX("Dodano ") + CMyString(dataProvider->getIleDodano()) + _T(",zaktualizowano ") + CMyString(dataProvider->getIleZaktualizowano()) + _T(" upustów"));

    CMyDialog::OnOK();
}

void CImportUpustyD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CImportDataProviderC>& prm)
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
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s cen indywidualnych z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        while (!(*dp)->isEOF() && !sr->isAborded())
        {
            if (osobneTransakcje) ct.BeginTransaction();
            linia++;

            {
                CCommonUpustImport cti;
                cti.setMapowanieKodu((eTowarSearchBy)_atoi(DTGDTGETPOLET("_mapKod")));
                cti.setNrLinii(linia);
                cti.setStatusRetHandler(sr);
                cti.setImportCenZakupu(_atoi(DTGDTGETPOLET("rodzaj_cen")));
                cti.setHowActualize(getHowActualize());
                cti.setKeysCache(keysCache);
                cti.setDataProvider(*dp);

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

        CDataT::toLog(_FX(_TX("Zakończono import %s cen indywidualnych z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
    }
}


eHowActualize CImportUpustyD::getHowActualize()
{
    return (eHowActualize)_atoi(DTGDTGETPOLET("_onexists"));
}


CUInfo CImportUpustyD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportUpustyD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_UPUSTY));
}

INT_PTR CImportUpustyD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_UPUSTY))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
