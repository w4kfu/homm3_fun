Snd\_extractor is a tool for extracting audio (.wav) files from .snd files that you can find in Heroes of Might and Magic 3

## Snd Format

* +0x00 : NbWavFile (DWORD)
* +0x04 : First WavEntry
* +0x34 : Second WavEntry
* ... Repeat NbWavFile
* +0x.. : WavFile

## Wav Entry

* +0x00 : NameFile (BYTE [40])
* +0x28 : Offset (DWORD)
* +0x2C : Size (DWORD)

## Example (H3ab\_ahd.snd)

	+0x00000000 : 75 00 00 00 41 7A 75 72 61 74 74 6B 00 77 61 76 
	+0x00000010 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
	+0x00000020 : 00 00 00 00 00 00 00 00 00 00 00 00 F4 15 00 00 
	+0x00000030 : 48 32 00 00

* Number of Wav File : 0x75 (117) 
* NameFile           : Azurattk.wav (Be careful "." is \x00)
* Offset             : 0x000015F4 (5620)
* Size               : 0x00003248 (12872)

	+0x000015F4 : 52 49 46 46 40 32 00 00 57 41 56 45 66 6D 74 20 
	+0x00001604 : 14 00 00 00 11 00 01 00 22 56 00 00 5C 2B 00 00 
	+0x00001614 : 00 02 04 00 02 00 F9 03 66 61 63 74 04 00 00 00 
	+0x00001624 : 60 63 00 00 64 61 74 61 0C 32 00 00

* FileTypeBlocID : "RIFF"  (0x52,0x49,0x46,0x46)
* FileSize       : Size - 8 bytes
* FileFormatID   : "WAVE"  (0x57,0x41,0x56,0x45)
...
