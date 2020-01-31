#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "utility.hpp"

using namespace riscv_isa;


class SectionHeader {
public:
    enum SectionHeaderType : u32 {
        Null = 0,           /// Marks an unused section header
        ProgramBits = 1,    /// Contains information defined by the program
        SymbolTable = 2,    /// Contains a linker symbol table
        StringTable = 3,    /// Contains a string table
        Rela = 4,           /// Contains “Rela” type relocation entries
        Hash = 5,           /// Contains a symbol hash table
        Dynamic = 6,        /// Contains dynamic linking tables
        Note = 7,           /// Contains note information
        NoBits = 8,         /// Contains uninitialized space; does not occupy any space in the file
        Rel = 9,            /// Contains “Rel” type relocation entries
        Shlib = 10,         /// Reserved
        DynamicSymbol = 11, /// Contains a dynamic loader symbol table
    };

    ///	contains the offset, in bytes, to the section name, relative to the start of the section
    /// name string table.
    u32 name;
    /// identifies the section type.
    SectionHeaderType section_type;
    /// identifies the attributes of the section.
    u32 flags;
    /// contains the virtual address of the beginning of the section in memory. If the section is
    /// not allocated to the memory image of the program, this field should be zero.
    u32 address;
    /// contains the offset, in bytes, of the beginning of the section contents in the file.
    u32 offset;
    /// contains the size, in bytes, of the section. Except for ShtNoBits sections, this is the
    /// amount of space occupied in the file.
    u32 size;
    /// contains the section index of an associated section. This field is used for several
    /// purposes, depending on the type of section, as explained in Table 10.
    u32 link;
    /// contains extra information about the section. This field is used for several purposes,
    /// depending on the type of section, as explained in Table 11.
    u32 info;
    /// contains the required alignment of the section. This field must be a power of two.
    u32 alignment;
    /// contains the size, in bytes, of each entry, for sections that contain fixed-size entries.
    /// Otherwise, this field contains zero.
    u32 entry_size;
};

class SectionHeaderIterator {
private:
    SectionHeader *inner;

public:
    explicit SectionHeaderIterator(SectionHeader *inner) : inner{inner} {}

};

class ELF32Header {
public:
    enum ELFClass : u8 {
        Bits32 = 1,
        Bits64 = 2,
    };

    friend std::ostream &operator<<(std::ostream &stream, ELFClass self) {
        switch (self) {
            case Bits32:
                stream << "Bits32";
                break;
            case Bits64:
                stream << "Bits64";
                break;
            default:
                stream << "???";
        }

        return stream;
    }

    enum DataEncoding : u8 {
        LittleEndian = 1,
        BigEndian = 2,
    };

    friend std::ostream &operator<<(std::ostream &stream, DataEncoding self) {
        switch (self) {
            case LittleEndian:
                stream << "LittleEndian";
                break;
            case BigEndian:
                stream << "BigEndian";
                break;
            default:
                stream << "???";
        }

        return stream;
    }

    enum OsAbi : u8 {
        SystemV = 0,
        HpUx = 1,
        Standalone = 255,
    };

    friend std::ostream &operator<<(std::ostream &stream, OsAbi self) {
        switch (self) {
            case SystemV:
                stream << "SystemV";
                break;
            case HpUx:
                stream << "HpUx";
                break;
            case Standalone:
                stream << "Standalone";
                break;
            default:
                stream << "???";
        }

        return stream;
    }

    enum ObjectFileType : u8 {
        None = 0,
        Relocatable = 1,
        Executable = 2,
        Shared = 3,
        Core = 4,
    };

    friend std::ostream &operator<<(std::ostream &stream, ObjectFileType self) {
        switch (self) {
            case None:
                stream << "None";
                break;
            case Relocatable:
                stream << "Relocatable";
                break;
            case Executable:
                stream << "Executable";
                break;
            case Shared:
                stream << "Shared";
                break;
            case Core:
                stream << "Core";
                break;
            default:
                stream << "???";
        }

        return stream;
    }

    /// contain a “magic number,” identifying the file as an ELF object file. They contain the
    /// characters ‘\x7f’, ‘E’, ‘L’, and ‘F’, respectively.
    char magic_number[4];
    /// identifies the class of the object file, or its capacity.
    ///
    /// The class of the ELF file is independent of the data model assumed by the object code. The
    /// elf_class field identifies the file format; a processor-specific flag in the flags field,
    /// described below, may be used to identify the application’s data model if the processory
    /// supports multiple models.
    ELFClass elf_class;
    /// specifies the data encoding of the object file data structures.
    ///
    /// For the convenience of code that examines ELF object files at run time (e.g., the dynamic
    /// loader), it is intended that the data encoding of the object file will match that of the
    /// running program. For environments that support both byte orders, a processor-specific flag
    /// in the flags field may be used to identify the application’s operating mode.
    DataEncoding data_encoding;
    /// identifies the version of the object file format.
    u8 identification_version;
    /// identifies the operating system and ABI for which the object is prepared. Some fields in
    /// other ELF structures have flags and values that have environment-specific meanings; the
    /// interpretation of those fields is determined by the value of this field.
    OsAbi os_abi;
    /// identifies the version of the ABI for which the object is prepared. This field is used
    /// to distinguish among incompatible versions of an ABI. The interpretation of this version
    /// number is dependent on the ABI identified by the os_abi field.
    u8 abi_version;
    /// The remaining bytes are reserved for future use, and should be set to zero.
    u8 _reserve[7];
    /// identifies the object file type.
    ObjectFileType file_type;
    /// identifies the target architecture. These values are defined in the processor-specific
    /// supplements.
    u16 machine_type;
    /// identifies the version of the object file format.
    u32 version;
    /// contains the virtual address of the program entry point. If there is no entry point, this
    /// field contains zero.
    u32 entry_point;
    /// contains the file offset, in bytes, of the program header table.
    u32 program_header_offset;
    /// contains the file offset, in bytes, of the section header table.
    u32 section_header_offset;
    /// contains processor-specific flags.
    u32 flags;
    /// contains the size, in bytes, of the ELF header.
    u16 elf_header_size;
    /// contains the size, in bytes, of a program header table entry.
    u16 program_header_size;
    /// contains the number of entries in the program header table.
    u16 program_header_num;
    /// contains the size, in bytes, of a section header table entry.
    u16 section_header_size;
    /// contains the number of entries in the section header table.
    u16 section_header_num;
    /// contains the section header table index of the section containing the section name string
    /// table. If there is no section name string table, this field has the value SHN_UNDEF.
    u16 string_table_index;

    ELF32Header(const ELF32Header &other) = delete;

    ELF32Header &operator=(const ELF32Header &other) = delete;

    friend std::ostream &operator<<(std::ostream &stream, const ELF32Header &self) {
        stream << "class ELF64Header {\n";
        stream << "\tmagic_number: " << self.magic_number << '\n';
        stream << "\telf_class: " << self.elf_class << '\n';
        stream << "\tdata_encoding: " << self.data_encoding << '\n';
        stream << "\tidentification_version: " << static_cast<u32>(self.identification_version) << '\n';
        stream << "\tos_abi: " << self.os_abi << '\n';
        stream << "\tabi_version: " << static_cast<u32>(self.abi_version) << '\n';
        stream << "\tfile_type: " << self.file_type << '\n';
        stream << "\tmachine_type: " << self.machine_type << '\n';
        stream << "\tversion: " << self.version << '\n';
        stream << "\tentry_point: " << self.entry_point << '\n';
        stream << "\tprogram_header_offset: " << self.program_header_offset << '\n';
        stream << "\tsection_header_offset: " << self.section_header_offset << '\n';
        stream << "\tflags: " << self.flags << '\n';
        stream << "\telf_header_size: " << self.elf_header_size << '\n';
        stream << "\tprogram_header_size: " << self.program_header_size << '\n';
        stream << "\tprogram_header_num: " << self.program_header_num << '\n';
        stream << "\tsection_header_size: " << self.section_header_size << '\n';
        stream << "\tsection_header_num: " << self.section_header_num << '\n';
        stream << "\tstring_table_index: " << self.string_table_index << '\n';
        stream << '}';

        return stream;
    }
};


int main(int argc, char **argv) {
    if (argc != 2) std::cout << "receive one file name!" << std::endl;

    int fd = open(argv[1], O_RDONLY);

    if (fd == -1) std::cout << "open file fail!" << std::endl;

    struct stat file_stat{};

    if (fstat(fd, &file_stat) != 0) std::cout << "fstat file fail!" << std::endl;

    void *file = mmap(nullptr, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);

    ELF32Header *elf_header = reinterpret_cast<ELF32Header *>(file);

    std::cout << *elf_header << std::endl;
}
