/*
    MOD: PNH_Core 2.0
    SCRIPT: PNH_EventsWorldData.c (4_World)
    DESC: Base de dados de coordenadas para sorteio de missões PNH.
*/
class PNH_EventsWorldData
{
    static ref array<string> MissionEvents = new array<string>;
    static ref array<vector> MissionPositions = new array<vector>;

    static void Init()
    {
        if (MissionEvents.Count() > 0) return; // Evita carregar duas vezes

        // =========================================================================
        // --- LISTA DE MISSÕES (Corrigidas com .ToVector()) ---
        // =========================================================================
        
        // --- APARTAMENTOS ---
        MissionEvents.Insert("Apartment Berezino north TenSmall_1");   MissionPositions.Insert("12166.4 20.8 9692.8".ToVector());
        MissionEvents.Insert("Apartment Berezino north TenSmall_2");   MissionPositions.Insert("12144.5 20.8 9698.3".ToVector());
        MissionEvents.Insert("Apartment Chernogorsk central TenSmall_1"); MissionPositions.Insert("6436.6 18.5 2765.3".ToVector());
        MissionEvents.Insert("Apartment Elektrozavodsk west TenSmall_1"); MissionPositions.Insert("9994.5 17.3 2106.6".ToVector());

        // --- CAÇA AO URSO (BearHunt) ---
        MissionEvents.Insert("BearHunt Black_Lake west Berezino");      MissionPositions.Insert("13192.0 93.0 11995.0".ToVector());
        MissionEvents.Insert("BearHunt Bor northwest Vysotovo");       MissionPositions.Insert("3597.8 215.7 4011.3".ToVector());

        // --- HORDAS (Horde) ---
        MissionEvents.Insert("Horde Balota west Residential_1");       MissionPositions.Insert("4400.5 7.3 2517.7".ToVector());
        MissionEvents.Insert("Horde Balota west Residential_2");       MissionPositions.Insert("4425.0 11.9 2466.1".ToVector());
        MissionEvents.Insert("Horde Bor north Residential_1");         MissionPositions.Insert("3107.7 218.4 4039.7".ToVector());

        // --- QUEDAS DE AVIÃO (PlaneCrash) ---
        MissionEvents.Insert("PlaneCrash Balota_Airstrip central");    MissionPositions.Insert("4934.0 9.5 2551.1".ToVector());
        MissionEvents.Insert("PlaneCrash Elektrozavodsk west");        MissionPositions.Insert("9140.1 5.9 2010.6".ToVector());

        // --- TRANSPORTE (Transport) ---
        MissionEvents.Insert("Transport Vybor to Altar");             MissionPositions.Insert("4202.2 324.4 8963.4".ToVector());

        // --- CIDADES/LOJAS (CityStore) ---
        MissionEvents.Insert("CityStore Berezino central Dolina");     MissionPositions.Insert("12029.0 55.4 9161.8".ToVector());
        MissionEvents.Insert("CityStore Elektrozavodsk central Dolina"); MissionPositions.Insert("10423.1 7.4 2364.9".ToVector());

        // --- CEMITÉRIOS (Graveyard) ---
        MissionEvents.Insert("Graveyard Berezino west Stash_1");       MissionPositions.Insert("13006.3 6.2 10575.4".ToVector());
        
        // Podes continuar a adicionar as tuas centenas de coordenadas aqui, 
        // seguindo sempre o padrão: NomeDaClasse + Espaço + Descrição + .ToVector()
        
    } // Fecha Init

    static bool GetBuildingsAtLoc(string bldName, string desc, out array<vector> posList)
    {
        if (!posList) posList = new array<vector>;
        return false; 
    }
}