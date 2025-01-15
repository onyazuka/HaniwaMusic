#ifndef M3UPLAYLIST_HPP
#define M3UPLAYLIST_HPP
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace m3u {

    class IM3UEntry {
    public:
        virtual ~IM3UEntry();
        virtual void dump (std::ostream& os) const = 0;
    };

    class EXTINF : public IM3UEntry {
    public:
        EXTINF(size_t durationS, const std::string& title = "");
        inline size_t durationS() const { return _durationS; }
        inline const std::string& title() const { return _title; }
        void dump(std::ostream& os) const override;
    private:
        const size_t _durationS;
        const std::string _title;
    };

    class PLAYLIST : public IM3UEntry {
    public:
        PLAYLIST(const std::string& title);
        inline const std::string& title() const { return _title; }
        void dump(std::ostream& os) const override;
    private:
        std::string _title;
    };

    struct M3UEntry {
        std::optional<EXTINF> oExtinf;
        std::optional<PLAYLIST> oPlaylist;
        std::string path;
        void dump(std::ostream& os) const;
    };

    class M3UPlaylist {
    public:
        void add(const M3UEntry& entry);
        inline const std::vector<M3UEntry>& entries() const { return _entries; }
        void dump(std::ostream& os) const;
        static M3UPlaylist fromStream(std::istream& is);
    private:
        std::vector<M3UEntry> _entries;
    };

    class M3UWriter {
    public:
        M3UWriter();
        M3UWriter& operator<<(const EXTINF& extinf);
        M3UWriter& operator<<(const PLAYLIST& playlist);
        M3UWriter& operator<<(const std::string& path);
        const M3UPlaylist& playlist() const { return _playlist; }
        bool dumpToFile(const std::string& path) const;
        std::string dumpToString() const;
    private:
        std::shared_ptr<M3UEntry> entry;
        M3UPlaylist _playlist;
    };

}

#endif // M3UPLAYLIST_HPP
