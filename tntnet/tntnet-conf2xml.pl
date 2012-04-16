#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 OPTIONS

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
my ($mappings, $listeners, $ssllisteners, $settings, $appconfigs, $comppath) = ("", "", "", "", "");

my %keywords = (
    PropertyFile => 'logproperties',
    MaxRequestSize => 'maxRequestSize',
    MaxRequestTime => 'maxRequestTime',
    User => 'user',
    Group => 'group',
    Dir => 'dir',
    Chroot => 'chrootdir',
    PidFile => 'pidFile',
    Daemon => 'daemon',
    MinThreads => 'minThreads',
    MaxThreads => 'maxThreads',
    ThreadStartDelay => 'threadStartDelay',
    QueueSize => 'queueSize',
    CompPath => 'compPath',
    BufferSize => 'socketBufferSize',
    SocketReadTimeout => 'socketReadTimeout',
    SocketWriteTimeout => 'socketWriteTimeout',
    KeepAliveTimeout => 'keepAliveTimeout',
    KeepAliveMax => 'keepAliveMax',
    SessionTimeout => 'sessionTimeout',
    ListenBacklog => 'listenBacklog',
    ListenRetry => 'listenRetry',
    EnableCompression => 'enableCompression',
    MimeDb => 'mimeDb',
    MinCompressSize => 'minCompressSize',
    MaxUrlMapCache => 'maxUrlMapCache',
    DefaultContentType => 'defaultContentType',
    AccessLog => 'accessLog',
    ErrorLog => 'errorLog',
    MaxBackgroundTasks => 'maxBackgroundTasks',
    MimeDb => 'mimeDb',
    DocumentRoot => 'documentRoot',
);

while (<>)
{
  chomp;
  s/ *#.*//g;
  my ($keyword, @params) = split(/\s+/);
  next unless $keyword;

  if ($keyword =~ /^(V?)MapUrl$/)
  {
    my $vhost = shift @params if ($1);
    my ($url, $target, $pathinfo, @args) = @params;
    $mappings .= <<EOF;
    <mapurl>
      <target>$target</target>
      <url>$url</url>
EOF
    $mappings .= <<EOF if $pathinfo;
      <pathinfo>$pathinfo</pathinfo>
EOF
    $mappings .= <<EOF if $vhost;
      <vhost>$vhost</vhost>
EOF
    if (@args)
    {
      $mappings .= <<EOF;
      <args>
EOF
      foreach (@args)
      {
        $mappings .= <<EOF;
        <arg>$_</arg>
EOF
      }

      $mappings .= <<EOF;
      </args>
EOF

    }
    $mappings .= <<EOF;
    </mapurl>
EOF
  }
  elsif ($keyword eq "Listen")
  {
    my ($ip, $port) = @params;
    $ip =~ s/"//g;
    $listeners .= <<EOF;
    <listener>
      <ip>$ip</ip>
      <port>$port</port>
    </listener>
EOF
  }
  elsif ($keyword eq "SslListen")
  {
    my ($ip, $port, $certificate, $key) = @params;
    $ip =~ s/"//g;
    $ssllisteners .= <<EOF;
    <sslistener>
      <ip>$ip</ip>
      <port>$port</port>
      <certificate>$certificate</certificate>
EOF
    $ssllisteners .= <<EOF if $key;
      <key>$key</key>
EOF
    $ssllisteners .= <<EOF;
    </sslistener>
EOF
  }
  elsif ($keyword eq "CompPath")
  {
    $comppath .= <<EOF;
    <entry>$params[0]</entry>
EOF
  }
  elsif ($keywords{$keyword})
  {
    $settings .= <<EOF;
  <$keywords{$keyword}>$params[0]</$keywords{$keyword}>
EOF
  }
  else
  {
    $appconfigs .= <<EOF;
  <$keyword>$params[0]</$keyword>
EOF
  }
}

print <<EOF;
<?xml version="1.0" encoding="UTF-8"?>
<tntnet>
  <mappings>
$mappings  </mappings>
EOF
print <<EOF if $listeners;
  <listeners>
$listeners  </listeners>
EOF
print <<EOF if $ssllisteners;
  <ssllisteners>
$ssllisteners  </ssllisteners>
EOF
print $settings;
print <<EOF if $comppath;
  <compPath>
$comppath  </compPath>
EOF
print <<EOF if $appconfigs;
  <appconfig>
$appconfigs  </appconfig>
EOF
print <<EOF;
</tntnet>
EOF
