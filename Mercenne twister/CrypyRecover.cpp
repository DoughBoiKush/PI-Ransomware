#include "utile.h"
#include <iostream>
#include "Matrice.h"
#include "CrypyRecover.h"

#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "cryptopp/modes.h"
#include "cryptopp/hex.h"
#include "cryptopp/cryptlib.h"

CrypyRecover::CrypyRecover(string pathRacine)
: extensions({"txt", "exe", "php", "pl", "7z", "rar", "m4a", "wma", "avi", "wmv", "csv", "d3dbsp", "sc2save", "sie", "sum", "ibank", "t13", "t12", "qdf", "gdb", "tax", "pkpass", "bc6", "bc7", "bkp",
              "qic", "bkf", "sidn", "sidd", "mddata", "itl", "itdb", "icxs", "hvpl", "hplg", "hkdb", "mdbackup", "syncdb", "gho", "cas", "svg", "map", "wmo", "itm", "sb", "fos", "mcgame", "vdf",
              "ztmp", "sis", "sid", "ncf", "menu", "layout", "dmp", "blob", "esm", "001", "vtf", "dazip", "fpk", "mlx", "kf", "iwd", "vpk", "tor", "psk", "rim", "w3x", "fsh", "ntl", "arch00", "lvl",
              "snx", "cfr", "ff", "vpp_pc", "lrf", "m2", "mcmeta", "vfs0", "mpqge", "kdb", "db0", "mp3", "upx", "rofl", "hkx", "bar", "upk", "das", "iwi", "litemod", "asset", "forge", "ltx", "bsa",
              "apk", "re4", "sav", "lbf", "slm", "bik", "epk", "rgss3a", "pak", "big", "unity3d", "wotreplay", "xxx", "desc", "py", "m3u", "flv", "js", "css", "rb", "png", "jpeg", "p7c", "p7b", "p12",
              "pfx", "pem", "crt", "cer", "der", "x3f", "srw", "pef", "ptx", "r3d", "rw2", "rwl", "raw", "raf", "orf", "nrw", "mrwref", "mef", "erf", "kdc", "dcr", "cr2", "crw", "bay", "sr2", "srf",
              "arw", "3fr", "dng", "jpeg", "jpg", "cdr", "indd", "ai", "eps", "pdf", "pdd", "psd", "dbfv", "mdf", "wb2", "rtf", "wpd", "dxg", "xf", "dwg", "pst", "accdb", "mdb", "pptm", "pptx", "ppt",
              "xlk", "xlsb", "xlsm", "xlsx", "xls", "wps", "docm", "docx", "doc", "odb", "odc", "odm", "odp", "ods", "odt", "sql", "zip", "tar", "tar.gz", "tgz", "biz", "ocx", "html", "htm", "3gp",
              "srt", "cpp", "mid", "mkv", "mov", "asf", "mpeg", "vob", "mpg", "fla", "swf", "wav", "qcow2", "vdi", "vmdk", "vmx", "gpg", "aes", "ARC", "PAQ", "tar.bz2", "tbk", "bak", "djv", "djvu",
              "bmp", "cgm", "tif", "tiff", "NEF", "cmd", "class", "jar", "java", "asp", "brd", "sch", "dch", "dip", "vbs", "asm", "pas", "ldf", "ibd", "MYI", "MYD", "frm", "dbf", "SQLITEDB", "SQLITE3",
              "asc", "lay6", "lay", "ms11 (Security copy)", "sldm", "sldx", "ppsm", "ppsx", "ppam", "docb", "mml", "sxm", "otg", "slk", "xlw", "xlt", "xlm", "xlc", "dif", "stc", "sxc", "ots", "ods",
              "hwp", "dotm", "dotx", "docm", "DOT", "max", "xml", "uot", "stw", "sxw", "ott", "csr", "key"}),
   mercenneSlayer(32, 624, 397, 31, 0x9908B0DF, 11, 0xFFFFFFFF, 7, 0x9D2C5680, 15, 0xEFC60000, 18)
{
    pathDechiffre = "tests/fichiers_crypy_dechiffres/";

    MercenneSlayer::tester();

    nbFichiers = nbElementsDans(pathRacine);
    
    if (nbFichiers < 624*4/16)
    {
        cout << "Pas assez de fichiers chiffres" << endl;
        return;
    }

    uint8_t randints[624*4];
    fichiers = fichiersEtDossiersDans(pathRacine);

    for (int i=0; i<624*4/16; i++) recupererIv(fichiers[i], randints+i*16);

    Matrice truc(randints, 624*32);
    cout << endl << truc.hexa() << endl;

    mercenneSlayer.sEtatSCrypy(randints);
    //mercenneSlayer.sEtatsSEtats("tests/exempleEtat");

    keys = new string[nbFichiers];
    for (int i=0; i<nbFichiers; i++) keys[i] = generateKey();

    cout << keys[0] << " " << keys[1] << " " << keys[2] << endl;
}

void CrypyRecover::recupererIv(string path, uint8_t iv[]) const
{
    ifstream fichier;
	fichier.open(path.c_str(), ios::binary);

	if (!fichier)
	{
	    cerr << "Error while opening file " << path << endl;
	    fichier.close();
	    return;
	}

	fichier.seekg(8, ios::beg);

	char iIv[17];
	fichier.read(iIv, 17);
	for (short i=0; i<16; i++) iv[i] = iIv[i];
}

void CrypyRecover::decipher(string path) const
{
    //On recupere le numero du fichier pour connaitre sa cle
    long nFichier = 0;
    while (fichiers[nFichier] != path && nFichier < nbFichiers) nFichier++;
    
    if (nFichier == nbFichiers)
    {
        cout << "Fichier inconnu" << endl;
        return;
    }

    string key = keys[nFichier];

    //On dechiffre le fichier
    ifstream fichier;
	fichier.open(path.c_str(), ios::binary);

	if (!fichier)
	{
	    cerr << "Error while opening file " << path << endl;
	    fichier.close();
	    return;
	}

	fichier.seekg(8, ios::beg);

    char iIv[17];
	fichier.read(iIv, 16);

    CryptoPP::byte iv[17];
    for (short i=0; i<16; i++) iv[i] = static_cast<uint8_t>(iIv[i]);
    
    CryptoPP::byte iKey[32];
    for (short i=0; i<32; i++) iKey[i] = static_cast<uint8_t>(key[i]);

    ofstream fNouveau;
    fNouveau.open((pathDechiffre+"fichier"+to_string(nFichier)).c_str(), ios::binary);

    char iDonnees[65537];
    fichier.read(iDonnees, 65536);
    int nbDonnees = fichier.gcount();

    while(nbDonnees > 0)
    {
        CryptoPP::byte donnees[65537];
        for (long i=0; i<nbDonnees; i++) donnees[i] = static_cast<uint8_t>(iDonnees[i]);

        CryptoPP::CBC_Mode<CryptoPP::AES >::Decryption d;
        d.SetKeyWithIV( iKey, 32, iv, 16);

        d.ProcessData(donnees, donnees, nbDonnees);

        fNouveau.write((char*)donnees, nbDonnees);

        fichier.read(iDonnees, 65536);
        nbDonnees = fichier.gcount();
    }

    fichier.close();
    fNouveau.close();
}

string CrypyRecover::generateKey()
{
    static char const caracteres[62] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
                                        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    string res = "";
    for (short i=0; i<32; i++) res += mercenneSlayer.choice(caracteres, 62);
    for (short i=0; i<52; i++) mercenneSlayer.randint(0, 255);

    return res;
}

CrypyRecover::~CrypyRecover()
{
    delete[] keys;
    delete[] fichiers;
}

//Fonctions statiques
void CrypyRecover::tester()
{
    CrypyRecover crypyRecover("tests/Documents/__SINTA I LOVE YOU__");
    crypyRecover.decipher("tests/Documents/__SINTA I LOVE YOU__/f3TvPsmcL7fDgsQpwZvuklET2PgjCziy723l.sinta");
    
    for (long i=0; i<100; i++) crypyRecover.decipher(crypyRecover.fichiers[i]);
}
