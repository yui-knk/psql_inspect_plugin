MRuby::Build.new('host') do |conf|
  # be sure to include this gem (the cli app)
  conf.gem File.expand_path(File.dirname(__FILE__))

  enable_debug

  toolchain :gcc

  conf.gembox 'full-core'

  conf.cc do |cc|
    # cc.flags << ENV['NGX_MRUBY_CFLAGS'] if ENV['NGX_MRUBY_CFLAGS']
  end

  conf.linker do |linker|
    # linker.flags << ENV['NGX_MRUBY_LDFLAGS'] if ENV['NGX_MRUBY_LDFLAGS']

    # when using openssl from brew
    if RUBY_PLATFORM =~ /darwin/i
      linker.flags << '-L/usr/local/opt/openssl/lib -lcrypto'
    end
  end
end

MRuby::Build.new('test') do |conf|
  # load specific toolchain settings

  # Gets set by the VS command prompts.
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  enable_debug

  # include the default GEMs
  conf.gembox 'full-core'
end
