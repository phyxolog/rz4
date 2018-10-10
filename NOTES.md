Header structure of packed file:

```text
Signature - 4 bytes, "RZ4F"
Original Size - 8 bytes
NumberOfStreams - 4 bytes
CompressedStreams[] -
  {
    Type - 8 bytes (Fixed String)
    Compressor - 8 bytes (Fixed String)
    Offset - 8 bytes
    CompressedSize - 8 bytes
    OriginalSize - 8 bytes
    OriginalCRC32 - 4 bytes
    CompressedData
  }
OriginalData
```

Size of CompressedStream node can be calculated by this formula:

```
(CompressedSize = CompressedData size)
(For quick jump to next node)
```

```
sizeof(Type) + sizeof(Compressor) + sizeof(Offset) + sizeof(CompressedSize)
+ sizeof(OriginalSize) + sizeof(OriginalSize) + sizeof(OriginalCRC32)
+ CompressedSize
```

Packing algorithm:

```
First off, we create empty output file.
Write header (without OriginalData, NumberOfStreams, CompressedStreams).
Start scanning file. When we found media stream - we should be check for
whether we can compress the flow or not. If all ok - compress, create CompressedStream
and fill him.

Example for RIFF WAVE stream:

Type - "RIFFWAVE"
Compressor - "WAVPACK"
Offset - 512
CompressedSize - 56899
OriginalSize - 108521
OriginalCRC32 - 0x814141AB
CompressedData - data of compressed via WAVPACK

Then - increment NumberOfStreams and write CompressedStream to output file.
After this manipulation we should be copy data from original file before
found stream. In this case we start from 0 and to 512. Copy data to
OriginalData field and save to out file.
```