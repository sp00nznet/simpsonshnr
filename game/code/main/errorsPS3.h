//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        errorsPS3.h
//
// Description: PS3-specific error messages for disc/drive errors
//
// History:     + Based on PS2 error messages
//
//=============================================================================

#ifndef ERRORSPS3_H
#define ERRORSPS3_H

// Error messages for PS3 disc errors
// These correspond to radFileError enum values

const char* ERROR_STRINGS[] =
{
    "",                                         // Success
    "File not found.",                          // FileNotFound
    "",                                         // ShellOpen (unused on PS3)
    "",                                         // WrongMedia
    "Please insert\nThe Simpsons: Hit & Run\nBlu-ray disc.",  // NoMedia
    "A disc read error has occurred.\nPlease check the disc.", // HardwareFailure
    "",                                         // (reserved)
    "",                                         // (reserved)
    "",                                         // (reserved)
    "",                                         // (reserved)
    "",                                         // (reserved)
    "",                                         // (reserved)
    ""                                          // (reserved)
};

// French error messages
const char* ERROR_STRINGS_FRENCH[] =
{
    "",
    "Fichier introuvable.",
    "",
    "",
    "Veuillez ins\xe9rer\nle disque Blu-ray\nLes Simpson: Le Jeu.",
    "Erreur de lecture.\nV\xe9rifiez le disque.",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
};

// German error messages
const char* ERROR_STRINGS_GERMAN[] =
{
    "",
    "Datei nicht gefunden.",
    "",
    "",
    "Bitte legen Sie die\nBlu-ray Disc\nDie Simpsons: Hit & Run ein.",
    "Lesefehler.\nBitte \xfcberpr\xfcfen Sie die Disc.",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
};

// Spanish error messages
const char* ERROR_STRINGS_SPANISH[] =
{
    "",
    "Archivo no encontrado.",
    "",
    "",
    "Por favor, inserte\nel disco Blu-ray de\nLos Simpson: Hit & Run.",
    "Error de lectura.\nCompruebe el disco.",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
};

#endif // ERRORSPS3_H
