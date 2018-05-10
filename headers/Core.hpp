class Core {
  public:
//    Core(){};
//    ~Core(){};
  private:
    virtual bool hotkeyLoop() { return false; };
    virtual bool interceptLoop() { return false; };
};
