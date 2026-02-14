class CfgPatches
{
	class PNH_Core
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"JM_CF_Scripts" // ADICIONADO: Necessário para o RestContext do Discord
		};
	};
};
class CfgMods
{
	class PNH_Core
	{
		dir="PNH_Core";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="PNH Core System";
		credits="Pinheiro";
		author="Pinheiro";
		authorID="0";
		version="1.0";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"PNH_Core/Scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"PNH_Core/Scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"PNH_Core/Scripts/5_Mission"
				};
			};
		};
	};
};
