#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iomanip>
#include <conio.h>

void ejectDrive(const char* drive_LETTER)
{
	wchar_t drive_name[10] = L"\\\\.\\?:";
	drive_name[4] = drive_LETTER[0];
	DWORD dwRet = 0;
	HANDLE hVol = CreateFile(drive_name, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hVol == INVALID_HANDLE_VALUE)
		return;

	if (!DeviceIoControl(hVol, FSCTL_LOCK_VOLUME, 0, 0, 0, 0, &dwRet, 0))
		return;

	if (!DeviceIoControl(hVol, FSCTL_DISMOUNT_VOLUME, 0, 0, 0, 0, &dwRet, 0))
		return;

	DeviceIoControl(hVol, IOCTL_STORAGE_EJECT_MEDIA, 0, 0, 0, 0, &dwRet, 0);

	CloseHandle(hVol);
}

const char* DRF[] = { "Unknown" , "Invalid path",
"Removable", "Fixed" , "Network drive","CD-ROM", "RAM disk" };

const char* get_drive_type(char* drive_letter)
{
	return DRF[GetDriveTypeA(drive_letter)];
}

bool isRemovableDrive(const char* drive_letter)
{
	if (strcmp(drive_letter, DRF[2]) == 0 || strcmp(drive_letter, DRF[5]) == 0)
	{
		return true;
	}
	return false;
}

void ejectDrive_Dialog()
{
	printf("  Введите букву извлекаемого устройства: ");
	char buffer[128];
	scanf("%s", buffer);
	buffer[1] = ':';
	buffer[2] = '\\';
	buffer[2] = '\0';
	const char* drive_type = get_drive_type(buffer);
	system("cls");
	if (isRemovableDrive(drive_type))
	{
		ejectDrive(buffer);
		printf("  Устройство %s извлечено!\n\n", buffer);
		return;
	}
	printf("  Устройство %s типа %s не может быть извлечено\n\n", buffer, drive_type);
}

void update()
{
	printf("Накопители: \n\n");
	char buf[26];
	GetLogicalDriveStringsA(sizeof(buf), buf);
	for (char* s = buf; *s; s += strlen(s) + 1)
	{
		const char* drive_type = get_drive_type(s);
		printf("%s  %s\n", s, drive_type);

		char VolumeNameBuffer[100];
		unsigned long MaximumComponentLength;
		unsigned long FileSystemFlags;
		char FileSystemNameBuffer[100];
		unsigned long VolumeSerialNumber;

		BOOL GetVolumeInformationFlag = GetVolumeInformationA(
			s,
			VolumeNameBuffer,
			100,
			&VolumeSerialNumber,
			&MaximumComponentLength,
			&FileSystemFlags,
			FileSystemNameBuffer,
			100
		);

		if (GetVolumeInformationFlag != 0)
		{
			printf("	Volume Name is %s\n", VolumeNameBuffer);
			printf("	Volume Serial Number is %lu\n", VolumeSerialNumber);
			printf("	Maximum Component Length is %lu\n", MaximumComponentLength);
			printf("	File System Flags is %lu\n", FileSystemFlags);
			printf("	File System is %s\n", FileSystemNameBuffer);

			unsigned long SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
			if (GetDiskFreeSpaceA(s, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
			{
				printf("	Sectors Per Cluster is %lu\n", SectorsPerCluster);
				printf("	Bytes Per Sector is %lu\n", BytesPerSector);
				printf("	Number Of Free Clusters is %lu\n", NumberOfFreeClusters);
				printf("	Total Number Of Clusters is %lu\n", TotalNumberOfClusters);
			}
		}
		else
		{
			printf("	(Пусто)\n", VolumeNameBuffer);
		}
	}
}

struct MENU
{
	void print()
	{
		printf("\n\nНажмите:\n  1   - чтобы извлечь устройство\n  2   - чтобы обновить список накопителей\n  Esc - чтобы закрыть программу\n\n");
	}

	void keystroke()
	{
		switch (_getch())
		{
		case '1':
			ejectDrive_Dialog();
			update_menu();
			break;
		case '2':
			system("cls");
			update_menu();
			break;
		case 27:
			exit(1);
			break;
		default:
			keystroke();
		}
	}

	void update_menu()
	{
		update();
		print();
		keystroke();
	}
};

int main()
{
	setlocale(LC_ALL, "russian");
	MENU menu;
	menu.update_menu();
	return 0;
}