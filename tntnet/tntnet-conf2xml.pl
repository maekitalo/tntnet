#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 OPTIONS

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
my ($mappings, $listeners, $settings, $appconfigs, $comppath) = ("", "", "", "", "");

my %keywords = (
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

my %obsolete = (
    PropertyFile => 'logproperties',
);

while (<>)
{
  chomp;
  s/ *#.*//g;
  my ($keyword, @params) = grep { defined($_) } /"([^"]*)"|([^"\s]+)/g;
  next unless $keyword;
  @params = map { s/^(["'])(.*)(\1)/$2/; $_; } @params;

  if ($keyword =~ /^(V?)MapUrl$/)
  {
    my $vhost = shift @params if ($1);
    my ($url, $target, $pathinfo, @args) = @params;
    $mappings .= <<EOF;
    <mapping>
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
    </mapping>
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
    $listeners .= <<EOF;
    <listener>
      <ip>$ip</ip>
      <port>$port</port>
      <certificate>$certificate</certificate>
EOF
    $listeners .= <<EOF if $key;
      <key>$key</key>
EOF
    $listeners .= <<EOF;
    </listener>
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
  elsif (!$obsolete{$keyword})
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
print <<EOF;
EOF
print <<EOF if $listeners;
  <listeners>
$listeners  </listeners>

EOF

print $settings;
print <<EOF if $comppath;
  <compPath>
$comppath  </compPath>
EOF

print <<EOF;
  <logging>
    <rootlogger>INFO</rootlogger>
    <loggers>
      <logger>
        <category>tntnet</category>
        <level>INFO</level>
      </logger>
    </loggers>
    <!-- <file>tntnet.log</file> -->      <!--uncomment if you want to log to a file -->
    <!-- <maxfilesize>1MB</maxfilesize> -->
    <!-- <maxbackupindex>2</maxbackupindex> -->
    <!-- <host>localhost:1234</host> --> <!--  # send log-messages with udp -->
  </logging>

$appconfigs</tntnet>
EOF
