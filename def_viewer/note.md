

* 0x40: sprite
* 0x41: spritedef
* 0x42: creature
* 0x43: advobj
* 0x44: hero
* 0x45: tileset
* 0x46: pointer
* 0x47: interface
* 0x48: sprite frame
* 0x49: combat hero
* 0x4F: advmask


## Example

Schools.def

Width  : 160    0xa0
Height : 96		0x60
Type : 47		71		0x2f
Compression : 1
Cadre : 4

.text:0059C2CD                 push    10h             ; int
.text:0059C2CF                 push    0               ; __int16
.text:0059C2D1                 push    0               ; char
.text:0059C2D3                 push    0               ; int
.text:0059C2D5                 push    0               ; int
.text:0059C2D7                 push    offset aSchools_def ; "schools.def"
.text:0059C2DC                 push    0ECh            ; int
.text:0059C2E1                 push    60h             ; Height
.text:0059C2E3                 push    0A0h            ; Width
.text:0059C2E8                 push    4Ah             ; int
.text:0059C2EA                 push    75h             ; int
.text:0059C2EC                 mov     ecx, eax
.text:0059C2EE                 call    sub_4EA720


0055C7B0 GetSprite proc near


    v4 = operator new(0x24u);
    *((_DWORD *)v4 + 1) = 0;
    *((_DWORD *)v4 + 8) = 1;
    dword_69E5B4 = (int)v4;
    *(_DWORD *)v4 = 0;
    *(_DWORD *)(dword_69E5B4 + 8) = 0;
	
	
.text:0055D305                 push    24h             ; unsigned int
.text:0055D307                 call    ??2@YAPAXI@Z    ; operator new(uint)
.text:0055D30C                 mov     [eax+4], ebx
.text:0055D30F                 mov     dword ptr [eax+20h], 1
.text:0055D316                 mov     dword_69E5B4, eax
.text:0055D31B                 mov     [eax], ebx
.text:0055D31D                 mov     ecx, dword_69E5B4
.text:0055D323                 add     esp, 4
.text:0055D326                 mov     [ecx+8], ebx