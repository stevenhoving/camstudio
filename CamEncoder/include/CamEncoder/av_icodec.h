#pragma once

class av_dict;

// interface for video codec.
class av_icodec
{
public:
    virtual void open(AVStream *stream, av_dict &dict) = 0;
    virtual bool pull_encoded_packet(AVPacket *pkt, bool *valid_packet) = 0;
    virtual AVCodecContext *get_codec_context() const noexcept = 0;
    virtual AVRational get_time_base() const noexcept = 0;
};