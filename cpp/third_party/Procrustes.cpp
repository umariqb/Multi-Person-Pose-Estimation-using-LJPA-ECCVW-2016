



<!DOCTYPE html>
<html lang="en" class="">
  <head prefix="og: http://ogp.me/ns# fb: http://ogp.me/ns/fb# object: http://ogp.me/ns/object# article: http://ogp.me/ns/article# profile: http://ogp.me/ns/profile#">
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta http-equiv="Content-Language" content="en">
    
    
    <title>Sandbox/Procrustes.cpp at master · subokita/Sandbox · GitHub</title>
    <link rel="search" type="application/opensearchdescription+xml" href="/opensearch.xml" title="GitHub">
    <link rel="fluid-icon" href="https://github.com/fluidicon.png" title="GitHub">
    <link rel="apple-touch-icon" sizes="57x57" href="/apple-touch-icon-114.png">
    <link rel="apple-touch-icon" sizes="114x114" href="/apple-touch-icon-114.png">
    <link rel="apple-touch-icon" sizes="72x72" href="/apple-touch-icon-144.png">
    <link rel="apple-touch-icon" sizes="144x144" href="/apple-touch-icon-144.png">
    <meta property="fb:app_id" content="1401488693436528">

      <meta content="@github" name="twitter:site" /><meta content="summary" name="twitter:card" /><meta content="subokita/Sandbox" name="twitter:title" /><meta content="Sandbox - Anything goes." name="twitter:description" /><meta content="https://avatars2.githubusercontent.com/u/2043928?v=2&amp;s=400" name="twitter:image:src" />
<meta content="GitHub" property="og:site_name" /><meta content="object" property="og:type" /><meta content="https://avatars2.githubusercontent.com/u/2043928?v=2&amp;s=400" property="og:image" /><meta content="subokita/Sandbox" property="og:title" /><meta content="https://github.com/subokita/Sandbox" property="og:url" /><meta content="Sandbox - Anything goes." property="og:description" />

      <meta name="browser-stats-url" content="/_stats">
    <link rel="assets" href="https://assets-cdn.github.com/">
    <link rel="conduit-xhr" href="https://ghconduit.com:25035">
    
    <meta name="pjax-timeout" content="1000">

    <meta name="msapplication-TileImage" content="/windows-tile.png">
    <meta name="msapplication-TileColor" content="#ffffff">
    <meta name="selected-link" value="repo_source" data-pjax-transient>
      <meta name="google-analytics" content="UA-3769691-2">

    <meta content="collector.githubapp.com" name="octolytics-host" /><meta content="collector-cdn.github.com" name="octolytics-script-host" /><meta content="github" name="octolytics-app-id" /><meta content="83DC9AB2:527E:4CC792B:5457DF88" name="octolytics-dimension-request_id" />
    
    <meta content="Rails, view, blob#show" name="analytics-event" />

    
    
    <link rel="icon" type="image/x-icon" href="https://assets-cdn.github.com/favicon.ico">


    <meta content="authenticity_token" name="csrf-param" />
<meta content="e8nPFV15MmPGz6XLcUkUs709gXhbdy22RiNhB+6s+wL6c9GdN5LVgE+fA80vZKMk9DVlEl9BE/qJSPSmRrtjPg==" name="csrf-token" />

    <link href="https://assets-cdn.github.com/assets/github-b1c905f1bdc31980aab4c315e0dd478fad5d2898a408bd7390807a27ba60160b.css" media="all" rel="stylesheet" type="text/css" />
    <link href="https://assets-cdn.github.com/assets/github2-61e6462a495b3342deeed7b5cea3ed71287442feb9f20aa1f6388cf860d513b5.css" media="all" rel="stylesheet" type="text/css" />
    
    


    <meta http-equiv="x-pjax-version" content="baa041f6fdc70489d4511a55a2ec39b9">

      
  <meta name="description" content="Sandbox - Anything goes.">
  <meta name="go-import" content="github.com/subokita/Sandbox git https://github.com/subokita/Sandbox.git">

  <meta content="2043928" name="octolytics-dimension-user_id" /><meta content="subokita" name="octolytics-dimension-user_login" /><meta content="17401327" name="octolytics-dimension-repository_id" /><meta content="subokita/Sandbox" name="octolytics-dimension-repository_nwo" /><meta content="true" name="octolytics-dimension-repository_public" /><meta content="false" name="octolytics-dimension-repository_is_fork" /><meta content="17401327" name="octolytics-dimension-repository_network_root_id" /><meta content="subokita/Sandbox" name="octolytics-dimension-repository_network_root_nwo" />
  <link href="https://github.com/subokita/Sandbox/commits/master.atom" rel="alternate" title="Recent Commits to Sandbox:master" type="application/atom+xml">

  </head>


  <body class="logged_out  env-production linux vis-public page-blob">
    <a href="#start-of-content" tabindex="1" class="accessibility-aid js-skip-to-content">Skip to content</a>
    <div class="wrapper">
      
      
      
      


      
      <div class="header header-logged-out" role="banner">
  <div class="container clearfix">

    <a class="header-logo-wordmark" href="https://github.com/" ga-data-click="(Logged out) Header, go to homepage, icon:logo-wordmark">
      <span class="mega-octicon octicon-logo-github"></span>
    </a>

    <div class="header-actions" role="navigation">
        <a class="button primary" href="/join" data-ga-click="(Logged out) Header, clicked Sign up, text:sign-up">Sign up</a>
      <a class="button signin" href="/login?return_to=%2Fsubokita%2FSandbox%2Fblob%2Fmaster%2FProcrustes%2FProcrustes%2FProcrustes.cpp" data-ga-click="(Logged out) Header, clicked Sign in, text:sign-in">Sign in</a>
    </div>

    <div class="site-search repo-scope js-site-search" role="search">
      <form accept-charset="UTF-8" action="/subokita/Sandbox/search" class="js-site-search-form" data-global-search-url="/search" data-repo-search-url="/subokita/Sandbox/search" method="get"><div style="margin:0;padding:0;display:inline"><input name="utf8" type="hidden" value="&#x2713;" /></div>
  <input type="text"
    class="js-site-search-field is-clearable"
    data-hotkey="s"
    name="q"
    placeholder="Search"
    data-global-scope-placeholder="Search GitHub"
    data-repo-scope-placeholder="Search"
    tabindex="1"
    autocapitalize="off">
  <div class="scope-badge">This repository</div>
</form>
    </div>

      <ul class="header-nav left" role="navigation">
          <li class="header-nav-item">
            <a class="header-nav-link" href="/explore" data-ga-click="(Logged out) Header, go to explore, text:explore">Explore</a>
          </li>
          <li class="header-nav-item">
            <a class="header-nav-link" href="/features" data-ga-click="(Logged out) Header, go to features, text:features">Features</a>
          </li>
          <li class="header-nav-item">
            <a class="header-nav-link" href="https://enterprise.github.com/" data-ga-click="(Logged out) Header, go to enterprise, text:enterprise">Enterprise</a>
          </li>
          <li class="header-nav-item">
            <a class="header-nav-link" href="/blog" data-ga-click="(Logged out) Header, go to blog, text:blog">Blog</a>
          </li>
      </ul>

  </div>
</div>



      <div id="start-of-content" class="accessibility-aid"></div>
          <div class="site" itemscope itemtype="http://schema.org/WebPage">
    <div id="js-flash-container">
      
    </div>
    <div class="pagehead repohead instapaper_ignore readability-menu">
      <div class="container">
        
<ul class="pagehead-actions">


  <li>
      <a href="/login?return_to=%2Fsubokita%2FSandbox"
    class="minibutton with-count star-button tooltipped tooltipped-n"
    aria-label="You must be signed in to star a repository" rel="nofollow">
    <span class="octicon octicon-star"></span>
    Star
  </a>

    <a class="social-count js-social-count" href="/subokita/Sandbox/stargazers">
      15
    </a>

  </li>

    <li>
      <a href="/login?return_to=%2Fsubokita%2FSandbox"
        class="minibutton with-count js-toggler-target fork-button tooltipped tooltipped-n"
        aria-label="You must be signed in to fork a repository" rel="nofollow">
        <span class="octicon octicon-repo-forked"></span>
        Fork
      </a>
      <a href="/subokita/Sandbox/network" class="social-count">
        14
      </a>
    </li>
</ul>

        <h1 itemscope itemtype="http://data-vocabulary.org/Breadcrumb" class="entry-title public">
          <span class="mega-octicon octicon-repo"></span>
          <span class="author"><a href="/subokita" class="url fn" itemprop="url" rel="author"><span itemprop="title">subokita</span></a></span><!--
       --><span class="path-divider">/</span><!--
       --><strong><a href="/subokita/Sandbox" class="js-current-repository js-repo-home-link" data-pjax-container-selector="#js-repo-pjax-container">Sandbox</a></strong>

          <span class="page-context-loader">
            <img alt="" height="16" src="https://assets-cdn.github.com/images/spinners/octocat-spinner-32.gif" width="16" />
          </span>

        </h1>
      </div><!-- /.container -->
    </div><!-- /.repohead -->

    <div class="container">
      <div class="repository-with-sidebar repo-container new-discussion-timeline  ">
        <div class="repository-sidebar clearfix">
            
<nav class="sunken-menu repo-nav js-repo-nav js-sidenav-container-pjax js-octicon-loaders" role="navigation" data-issue-count-url="/subokita/Sandbox/issues/counts" data-pjax-container-selector="#js-repo-pjax-container">
  <ul class="sunken-menu-group">
    <li class="tooltipped tooltipped-w" aria-label="Code">
      <a href="/subokita/Sandbox" aria-label="Code" class="selected js-selected-navigation-item sunken-menu-item" data-hotkey="g c" data-pjax="true" data-selected-links="repo_source repo_downloads repo_commits repo_releases repo_tags repo_branches /subokita/Sandbox">
        <span class="octicon octicon-code"></span> <span class="full-word">Code</span>
        <img alt="" class="mini-loader" height="16" src="https://assets-cdn.github.com/images/spinners/octocat-spinner-32.gif" width="16" />
</a>    </li>

      <li class="tooltipped tooltipped-w" aria-label="Issues">
        <a href="/subokita/Sandbox/issues" aria-label="Issues" class="js-selected-navigation-item sunken-menu-item js-disable-pjax" data-hotkey="g i" data-selected-links="repo_issues repo_labels repo_milestones /subokita/Sandbox/issues">
          <span class="octicon octicon-issue-opened"></span> <span class="full-word">Issues</span>
          <span class="js-issue-replace-counter"></span>
          <img alt="" class="mini-loader" height="16" src="https://assets-cdn.github.com/images/spinners/octocat-spinner-32.gif" width="16" />
</a>      </li>

    <li class="tooltipped tooltipped-w" aria-label="Pull Requests">
      <a href="/subokita/Sandbox/pulls" aria-label="Pull Requests" class="js-selected-navigation-item sunken-menu-item js-disable-pjax" data-hotkey="g p" data-selected-links="repo_pulls /subokita/Sandbox/pulls">
          <span class="octicon octicon-git-pull-request"></span> <span class="full-word">Pull Requests</span>
          <span class="js-pull-replace-counter"></span>
          <img alt="" class="mini-loader" height="16" src="https://assets-cdn.github.com/images/spinners/octocat-spinner-32.gif" width="16" />
</a>    </li>


      <li class="tooltipped tooltipped-w" aria-label="Wiki">
        <a href="/subokita/Sandbox/wiki" aria-label="Wiki" class="js-selected-navigation-item sunken-menu-item js-disable-pjax" data-hotkey="g w" data-selected-links="repo_wiki /subokita/Sandbox/wiki">
          <span class="octicon octicon-book"></span> <span class="full-word">Wiki</span>
          <img alt="" class="mini-loader" height="16" src="https://assets-cdn.github.com/images/spinners/octocat-spinner-32.gif" width="16" />
</a>      </li>
  </ul>
  <div class="sunken-menu-separator"></div>
  <ul class="sunken-menu-group">

    <li class="tooltipped tooltipped-w" aria-label="Pulse">
      <a href="/subokita/Sandbox/pulse/weekly" aria-label="Pulse" class="js-selected-navigation-item sunken-menu-item" data-pjax="true" data-selected-links="pulse /subokita/Sandbox/pulse/weekly">
        <span class="octicon octicon-pulse"></span> <span class="full-word">Pulse</span>
        <img alt="" class="mini-loader" height="16" src="https://assets-cdn.github.com/images/spinners/octocat-spinner-32.gif" width="16" />
</a>    </li>

    <li class="tooltipped tooltipped-w" aria-label="Graphs">
      <a href="/subokita/Sandbox/graphs" aria-label="Graphs" class="js-selected-navigation-item sunken-menu-item" data-pjax="true" data-selected-links="repo_graphs repo_contributors /subokita/Sandbox/graphs">
        <span class="octicon octicon-graph"></span> <span class="full-word">Graphs</span>
        <img alt="" class="mini-loader" height="16" src="https://assets-cdn.github.com/images/spinners/octocat-spinner-32.gif" width="16" />
</a>    </li>
  </ul>


</nav>

              <div class="only-with-full-nav">
                
  
<div class="clone-url open"
  data-protocol-type="http"
  data-url="/users/set_protocol?protocol_selector=http&amp;protocol_type=clone">
  <h3><span class="text-emphasized">HTTPS</span> clone URL</h3>
  <div class="input-group">
    <input type="text" class="input-mini input-monospace js-url-field"
           value="https://github.com/subokita/Sandbox.git" readonly="readonly">
    <span class="input-group-button">
      <button aria-label="Copy to clipboard" class="js-zeroclipboard minibutton zeroclipboard-button" data-clipboard-text="https://github.com/subokita/Sandbox.git" data-copied-hint="Copied!" type="button"><span class="octicon octicon-clippy"></span></button>
    </span>
  </div>
</div>

  
<div class="clone-url "
  data-protocol-type="subversion"
  data-url="/users/set_protocol?protocol_selector=subversion&amp;protocol_type=clone">
  <h3><span class="text-emphasized">Subversion</span> checkout URL</h3>
  <div class="input-group">
    <input type="text" class="input-mini input-monospace js-url-field"
           value="https://github.com/subokita/Sandbox" readonly="readonly">
    <span class="input-group-button">
      <button aria-label="Copy to clipboard" class="js-zeroclipboard minibutton zeroclipboard-button" data-clipboard-text="https://github.com/subokita/Sandbox" data-copied-hint="Copied!" type="button"><span class="octicon octicon-clippy"></span></button>
    </span>
  </div>
</div>


<p class="clone-options">You can clone with
      <a href="#" class="js-clone-selector" data-protocol="http">HTTPS</a>
      or <a href="#" class="js-clone-selector" data-protocol="subversion">Subversion</a>.
  <a href="https://help.github.com/articles/which-remote-url-should-i-use" class="help tooltipped tooltipped-n" aria-label="Get help on which URL is right for you.">
    <span class="octicon octicon-question"></span>
  </a>
</p>



                <a href="/subokita/Sandbox/archive/master.zip"
                   class="minibutton sidebar-button"
                   aria-label="Download the contents of subokita/Sandbox as a zip file"
                   title="Download the contents of subokita/Sandbox as a zip file"
                   rel="nofollow">
                  <span class="octicon octicon-cloud-download"></span>
                  Download ZIP
                </a>
              </div>
        </div><!-- /.repository-sidebar -->

        <div id="js-repo-pjax-container" class="repository-content context-loader-container" data-pjax-container>
          

<a href="/subokita/Sandbox/blob/9e1023a5150cb597097521728b3deeca73a2b386/Procrustes/Procrustes/Procrustes.cpp" class="hidden js-permalink-shortcut" data-hotkey="y">Permalink</a>

<!-- blob contrib key: blob_contributors:v21:c9c22bdfde0c774d84dcffa08a06d73d -->

<div class="file-navigation">
  
<div class="select-menu js-menu-container js-select-menu left">
  <span class="minibutton select-menu-button js-menu-target css-truncate" data-hotkey="w"
    data-master-branch="master"
    data-ref="master"
    title="master"
    role="button" aria-label="Switch branches or tags" tabindex="0" aria-haspopup="true">
    <span class="octicon octicon-git-branch"></span>
    <i>branch:</i>
    <span class="js-select-button css-truncate-target">master</span>
  </span>

  <div class="select-menu-modal-holder js-menu-content js-navigation-container" data-pjax aria-hidden="true">

    <div class="select-menu-modal">
      <div class="select-menu-header">
        <span class="select-menu-title">Switch branches/tags</span>
        <span class="octicon octicon-x js-menu-close" role="button" aria-label="Close"></span>
      </div> <!-- /.select-menu-header -->

      <div class="select-menu-filters">
        <div class="select-menu-text-filter">
          <input type="text" aria-label="Filter branches/tags" id="context-commitish-filter-field" class="js-filterable-field js-navigation-enable" placeholder="Filter branches/tags">
        </div>
        <div class="select-menu-tabs">
          <ul>
            <li class="select-menu-tab">
              <a href="#" data-tab-filter="branches" class="js-select-menu-tab">Branches</a>
            </li>
            <li class="select-menu-tab">
              <a href="#" data-tab-filter="tags" class="js-select-menu-tab">Tags</a>
            </li>
          </ul>
        </div><!-- /.select-menu-tabs -->
      </div><!-- /.select-menu-filters -->

      <div class="select-menu-list select-menu-tab-bucket js-select-menu-tab-bucket" data-tab-filter="branches">

        <div data-filterable-for="context-commitish-filter-field" data-filterable-type="substring">


            <div class="select-menu-item js-navigation-item ">
              <span class="select-menu-item-icon octicon octicon-check"></span>
              <a href="/subokita/Sandbox/blob/gh-pages/Procrustes/Procrustes/Procrustes.cpp"
                 data-name="gh-pages"
                 data-skip-pjax="true"
                 rel="nofollow"
                 class="js-navigation-open select-menu-item-text css-truncate-target"
                 title="gh-pages">gh-pages</a>
            </div> <!-- /.select-menu-item -->
            <div class="select-menu-item js-navigation-item selected">
              <span class="select-menu-item-icon octicon octicon-check"></span>
              <a href="/subokita/Sandbox/blob/master/Procrustes/Procrustes/Procrustes.cpp"
                 data-name="master"
                 data-skip-pjax="true"
                 rel="nofollow"
                 class="js-navigation-open select-menu-item-text css-truncate-target"
                 title="master">master</a>
            </div> <!-- /.select-menu-item -->
        </div>

          <div class="select-menu-no-results">Nothing to show</div>
      </div> <!-- /.select-menu-list -->

      <div class="select-menu-list select-menu-tab-bucket js-select-menu-tab-bucket" data-tab-filter="tags">
        <div data-filterable-for="context-commitish-filter-field" data-filterable-type="substring">


        </div>

        <div class="select-menu-no-results">Nothing to show</div>
      </div> <!-- /.select-menu-list -->

    </div> <!-- /.select-menu-modal -->
  </div> <!-- /.select-menu-modal-holder -->
</div> <!-- /.select-menu -->

  <div class="button-group right">
    <a href="/subokita/Sandbox/find/master"
          class="js-show-file-finder minibutton empty-icon tooltipped tooltipped-s"
          data-pjax
          data-hotkey="t"
          aria-label="Quickly jump between files">
      <span class="octicon octicon-list-unordered"></span>
    </a>
    <button class="js-zeroclipboard minibutton zeroclipboard-button"
          data-clipboard-text="Procrustes/Procrustes/Procrustes.cpp"
          aria-label="Copy to clipboard"
          data-copied-hint="Copied!">
      <span class="octicon octicon-clippy"></span>
    </button>
  </div>

  <div class="breadcrumb">
    <span class='repo-root js-repo-root'><span itemscope="" itemtype="http://data-vocabulary.org/Breadcrumb"><a href="/subokita/Sandbox" class="" data-branch="master" data-direction="back" data-pjax="true" itemscope="url"><span itemprop="title">Sandbox</span></a></span></span><span class="separator"> / </span><span itemscope="" itemtype="http://data-vocabulary.org/Breadcrumb"><a href="/subokita/Sandbox/tree/master/Procrustes" class="" data-branch="master" data-direction="back" data-pjax="true" itemscope="url"><span itemprop="title">Procrustes</span></a></span><span class="separator"> / </span><span itemscope="" itemtype="http://data-vocabulary.org/Breadcrumb"><a href="/subokita/Sandbox/tree/master/Procrustes/Procrustes" class="" data-branch="master" data-direction="back" data-pjax="true" itemscope="url"><span itemprop="title">Procrustes</span></a></span><span class="separator"> / </span><strong class="final-path">Procrustes.cpp</strong>
  </div>
</div>


  <div class="commit file-history-tease">
    <div class="file-history-tease-header">
        <img alt="" class="avatar" height="24" src="https://1.gravatar.com/avatar/802df12759ea1b8d634fd894c6a3593a?d=https%3A%2F%2Fassets-cdn.github.com%2Fimages%2Fgravatars%2Fgravatar-user-420.png&amp;r=x&amp;s=140" width="24" />
        <span class="author"><span>subokita</span></span>
        <time datetime="2014-09-22T12:22:04Z" is="relative-time">Sep 22, 2014</time>
        <div class="commit-title">
            <a href="/subokita/Sandbox/commit/9e1023a5150cb597097521728b3deeca73a2b386" class="message" data-pjax="true" title="Updated the search path to point to latest OpenCV. Not important for anyone else.">Updated the search path to point to latest OpenCV. Not important for …</a>
        </div>
    </div>

    <div class="participation">
      <p class="quickstat">
        <a href="#blob_contributors_box" rel="facebox">
          <strong>0</strong>
           contributors
        </a>
      </p>
      
    </div>
    <div id="blob_contributors_box" style="display:none">
      <h2 class="facebox-header">Users who have contributed to this file</h2>
      <ul class="facebox-user-list">
      </ul>
    </div>
  </div>

<div class="file-box">
  <div class="file">
    <div class="meta clearfix">
      <div class="info file-name">
          <span>221 lines (175 sloc)</span>
          <span class="meta-divider"></span>
        <span>6.125 kb</span>
      </div>
      <div class="actions">
        <div class="button-group">
          <a href="/subokita/Sandbox/raw/master/Procrustes/Procrustes/Procrustes.cpp" class="minibutton " id="raw-url">Raw</a>
            <a href="/subokita/Sandbox/blame/master/Procrustes/Procrustes/Procrustes.cpp" class="minibutton js-update-url-with-hash">Blame</a>
          <a href="/subokita/Sandbox/commits/master/Procrustes/Procrustes/Procrustes.cpp" class="minibutton " rel="nofollow">History</a>
        </div><!-- /.button-group -->


            <a class="octicon-button disabled tooltipped tooltipped-w" href="#"
               aria-label="You must be signed in to make or propose changes"><span class="octicon octicon-pencil"></span></a>

          <a class="octicon-button danger disabled tooltipped tooltipped-w" href="#"
             aria-label="You must be signed in to make or propose changes">
          <span class="octicon octicon-trashcan"></span>
        </a>
      </div><!-- /.actions -->
    </div>
    
  <div class="blob-wrapper data type-c">
      <table class="highlight tab-size-8 js-file-line-container">
      <tr>
        <td id="L1" class="blob-num js-line-number" data-line-number="1"></td>
        <td id="LC1" class="blob-code js-file-line"><span class="c1">//</span></td>
      </tr>
      <tr>
        <td id="L2" class="blob-num js-line-number" data-line-number="2"></td>
        <td id="LC2" class="blob-code js-file-line"><span class="c1">//  Procrustes.cpp</span></td>
      </tr>
      <tr>
        <td id="L3" class="blob-num js-line-number" data-line-number="3"></td>
        <td id="LC3" class="blob-code js-file-line"><span class="c1">//  Procrustes</span></td>
      </tr>
      <tr>
        <td id="L4" class="blob-num js-line-number" data-line-number="4"></td>
        <td id="LC4" class="blob-code js-file-line"><span class="c1">//</span></td>
      </tr>
      <tr>
        <td id="L5" class="blob-num js-line-number" data-line-number="5"></td>
        <td id="LC5" class="blob-code js-file-line"><span class="c1">//  Created by Saburo Okita on 07/04/14.</span></td>
      </tr>
      <tr>
        <td id="L6" class="blob-num js-line-number" data-line-number="6"></td>
        <td id="LC6" class="blob-code js-file-line"><span class="c1">//  Copyright (c) 2014 Saburo Okita. All rights reserved.</span></td>
      </tr>
      <tr>
        <td id="L7" class="blob-num js-line-number" data-line-number="7"></td>
        <td id="LC7" class="blob-code js-file-line"><span class="c1">//</span></td>
      </tr>
      <tr>
        <td id="L8" class="blob-num js-line-number" data-line-number="8"></td>
        <td id="LC8" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L9" class="blob-num js-line-number" data-line-number="9"></td>
        <td id="LC9" class="blob-code js-file-line"><span class="cp">#include &quot;Procrustes.h&quot;</span></td>
      </tr>
      <tr>
        <td id="L10" class="blob-num js-line-number" data-line-number="10"></td>
        <td id="LC10" class="blob-code js-file-line"><span class="cp">#include &lt;random&gt;</span></td>
      </tr>
      <tr>
        <td id="L11" class="blob-num js-line-number" data-line-number="11"></td>
        <td id="LC11" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L12" class="blob-num js-line-number" data-line-number="12"></td>
        <td id="LC12" class="blob-code js-file-line"><span class="k">using</span> <span class="k">namespace</span> <span class="n">std</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L13" class="blob-num js-line-number" data-line-number="13"></td>
        <td id="LC13" class="blob-code js-file-line"><span class="k">using</span> <span class="k">namespace</span> <span class="n">cv</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L14" class="blob-num js-line-number" data-line-number="14"></td>
        <td id="LC14" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L15" class="blob-num js-line-number" data-line-number="15"></td>
        <td id="LC15" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L16" class="blob-num js-line-number" data-line-number="16"></td>
        <td id="LC16" class="blob-code js-file-line"><span class="n">Procrustes</span><span class="o">::</span><span class="n">Procrustes</span><span class="p">()</span> <span class="o">:</span></td>
      </tr>
      <tr>
        <td id="L17" class="blob-num js-line-number" data-line-number="17"></td>
        <td id="LC17" class="blob-code js-file-line"><span class="n">scale</span><span class="p">(</span><span class="mf">1.0f</span><span class="p">),</span> <span class="n">error</span><span class="p">(</span><span class="mf">0.0f</span><span class="p">),</span> <span class="n">scaling</span><span class="p">(</span> <span class="nb">true</span> <span class="p">),</span> <span class="n">bestReflection</span><span class="p">(</span> <span class="nb">true</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L18" class="blob-num js-line-number" data-line-number="18"></td>
        <td id="LC18" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L19" class="blob-num js-line-number" data-line-number="19"></td>
        <td id="LC19" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L20" class="blob-num js-line-number" data-line-number="20"></td>
        <td id="LC20" class="blob-code js-file-line"><span class="n">Procrustes</span><span class="o">::</span><span class="n">Procrustes</span><span class="p">(</span> <span class="kt">bool</span> <span class="n">use_scaling</span><span class="p">,</span> <span class="kt">bool</span> <span class="n">best_reflection</span> <span class="p">)</span> <span class="o">:</span> <span class="n">scale</span><span class="p">(</span><span class="mf">1.0f</span><span class="p">),</span> <span class="n">error</span><span class="p">(</span><span class="mf">0.0f</span><span class="p">),</span> <span class="n">scaling</span><span class="p">(</span> <span class="n">use_scaling</span> <span class="p">),</span> <span class="n">bestReflection</span><span class="p">(</span> <span class="n">best_reflection</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L21" class="blob-num js-line-number" data-line-number="21"></td>
        <td id="LC21" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L22" class="blob-num js-line-number" data-line-number="22"></td>
        <td id="LC22" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L23" class="blob-num js-line-number" data-line-number="23"></td>
        <td id="LC23" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L24" class="blob-num js-line-number" data-line-number="24"></td>
        <td id="LC24" class="blob-code js-file-line"><span class="n">Procrustes</span><span class="o">::~</span><span class="n">Procrustes</span><span class="p">(){</span></td>
      </tr>
      <tr>
        <td id="L25" class="blob-num js-line-number" data-line-number="25"></td>
        <td id="LC25" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L26" class="blob-num js-line-number" data-line-number="26"></td>
        <td id="LC26" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L27" class="blob-num js-line-number" data-line-number="27"></td>
        <td id="LC27" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L28" class="blob-num js-line-number" data-line-number="28"></td>
        <td id="LC28" class="blob-code js-file-line"><span class="cm"> * Convert YPrime back from Mat to vector&lt;Point2f&gt;</span></td>
      </tr>
      <tr>
        <td id="L29" class="blob-num js-line-number" data-line-number="29"></td>
        <td id="LC29" class="blob-code js-file-line"><span class="cm"> * convenience method.</span></td>
      </tr>
      <tr>
        <td id="L30" class="blob-num js-line-number" data-line-number="30"></td>
        <td id="LC30" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L31" class="blob-num js-line-number" data-line-number="31"></td>
        <td id="LC31" class="blob-code js-file-line"><span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">yPrimeAsVector</span><span class="p">()</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L32" class="blob-num js-line-number" data-line-number="32"></td>
        <td id="LC32" class="blob-code js-file-line">    <span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L33" class="blob-num js-line-number" data-line-number="33"></td>
        <td id="LC33" class="blob-code js-file-line">    <span class="n">Yprime</span><span class="p">.</span><span class="n">copyTo</span><span class="p">(</span> <span class="n">result</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L34" class="blob-num js-line-number" data-line-number="34"></td>
        <td id="LC34" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L35" class="blob-num js-line-number" data-line-number="35"></td>
        <td id="LC35" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L36" class="blob-num js-line-number" data-line-number="36"></td>
        <td id="LC36" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L37" class="blob-num js-line-number" data-line-number="37"></td>
        <td id="LC37" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L38" class="blob-num js-line-number" data-line-number="38"></td>
        <td id="LC38" class="blob-code js-file-line"><span class="cm"> * Sum of squared of the 2 channels matrix</span></td>
      </tr>
      <tr>
        <td id="L39" class="blob-num js-line-number" data-line-number="39"></td>
        <td id="LC39" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L40" class="blob-num js-line-number" data-line-number="40"></td>
        <td id="LC40" class="blob-code js-file-line"><span class="kt">float</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">sumSquared</span><span class="p">(</span> <span class="k">const</span> <span class="n">Mat</span><span class="o">&amp;</span> <span class="n">mat</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L41" class="blob-num js-line-number" data-line-number="41"></td>
        <td id="LC41" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">temp</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L42" class="blob-num js-line-number" data-line-number="42"></td>
        <td id="LC42" class="blob-code js-file-line">    <span class="n">pow</span><span class="p">(</span> <span class="n">mat</span><span class="p">,</span> <span class="mf">2.0</span><span class="p">,</span> <span class="n">temp</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L43" class="blob-num js-line-number" data-line-number="43"></td>
        <td id="LC43" class="blob-code js-file-line">    <span class="n">Scalar</span> <span class="n">temp_scalar</span> <span class="o">=</span> <span class="n">sum</span><span class="p">(</span> <span class="n">temp</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L44" class="blob-num js-line-number" data-line-number="44"></td>
        <td id="LC44" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">temp_scalar</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span> <span class="o">+</span> <span class="n">temp_scalar</span><span class="p">[</span><span class="mi">1</span><span class="p">];</span></td>
      </tr>
      <tr>
        <td id="L45" class="blob-num js-line-number" data-line-number="45"></td>
        <td id="LC45" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L46" class="blob-num js-line-number" data-line-number="46"></td>
        <td id="LC46" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L47" class="blob-num js-line-number" data-line-number="47"></td>
        <td id="LC47" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L48" class="blob-num js-line-number" data-line-number="48"></td>
        <td id="LC48" class="blob-code js-file-line"><span class="cm"> * Wrapper for vector of points</span></td>
      </tr>
      <tr>
        <td id="L49" class="blob-num js-line-number" data-line-number="49"></td>
        <td id="LC49" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L50" class="blob-num js-line-number" data-line-number="50"></td>
        <td id="LC50" class="blob-code js-file-line"><span class="kt">float</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">procrustes</span><span class="p">(</span><span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;</span> <span class="o">&amp;</span><span class="n">X</span><span class="p">,</span> <span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;</span> <span class="o">&amp;</span><span class="n">Y</span><span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L51" class="blob-num js-line-number" data-line-number="51"></td>
        <td id="LC51" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">procrustes</span><span class="p">(</span> <span class="n">Mat</span><span class="p">(</span><span class="n">X</span><span class="p">),</span> <span class="n">Mat</span><span class="p">(</span><span class="n">Y</span><span class="p">));</span></td>
      </tr>
      <tr>
        <td id="L52" class="blob-num js-line-number" data-line-number="52"></td>
        <td id="LC52" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L53" class="blob-num js-line-number" data-line-number="53"></td>
        <td id="LC53" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L54" class="blob-num js-line-number" data-line-number="54"></td>
        <td id="LC54" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L55" class="blob-num js-line-number" data-line-number="55"></td>
        <td id="LC55" class="blob-code js-file-line"><span class="cm"> * Perform procrustes analysis / superimpose (?)</span></td>
      </tr>
      <tr>
        <td id="L56" class="blob-num js-line-number" data-line-number="56"></td>
        <td id="LC56" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L57" class="blob-num js-line-number" data-line-number="57"></td>
        <td id="LC57" class="blob-code js-file-line"><span class="kt">float</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">procrustes</span><span class="p">(</span> <span class="k">const</span> <span class="n">Mat</span><span class="o">&amp;</span> <span class="n">X</span><span class="p">,</span> <span class="k">const</span> <span class="n">Mat</span><span class="o">&amp;</span> <span class="n">Y</span> <span class="p">){</span></td>
      </tr>
      <tr>
        <td id="L58" class="blob-num js-line-number" data-line-number="58"></td>
        <td id="LC58" class="blob-code js-file-line">    <span class="cm">/* Recenter the points based on their mean ... */</span></td>
      </tr>
      <tr>
        <td id="L59" class="blob-num js-line-number" data-line-number="59"></td>
        <td id="LC59" class="blob-code js-file-line">    <span class="n">Scalar</span> <span class="n">mu_x</span> <span class="o">=</span> <span class="n">cv</span><span class="o">::</span><span class="n">mean</span><span class="p">(</span><span class="n">X</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L60" class="blob-num js-line-number" data-line-number="60"></td>
        <td id="LC60" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">X0</span>      <span class="o">=</span> <span class="n">X</span> <span class="o">-</span> <span class="n">Mat</span><span class="p">(</span><span class="n">X</span><span class="p">.</span><span class="n">size</span><span class="p">(),</span> <span class="n">X</span><span class="p">.</span><span class="n">type</span><span class="p">(),</span> <span class="n">mu_x</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L61" class="blob-num js-line-number" data-line-number="61"></td>
        <td id="LC61" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L62" class="blob-num js-line-number" data-line-number="62"></td>
        <td id="LC62" class="blob-code js-file-line">    <span class="n">Scalar</span> <span class="n">mu_y</span> <span class="o">=</span> <span class="n">cv</span><span class="o">::</span><span class="n">mean</span><span class="p">(</span><span class="n">Y</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L63" class="blob-num js-line-number" data-line-number="63"></td>
        <td id="LC63" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">Y0</span>      <span class="o">=</span> <span class="n">Y</span> <span class="o">-</span> <span class="n">Mat</span><span class="p">(</span><span class="n">Y</span><span class="p">.</span><span class="n">size</span><span class="p">(),</span> <span class="n">Y</span><span class="p">.</span><span class="n">type</span><span class="p">(),</span> <span class="n">mu_y</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L64" class="blob-num js-line-number" data-line-number="64"></td>
        <td id="LC64" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L65" class="blob-num js-line-number" data-line-number="65"></td>
        <td id="LC65" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L66" class="blob-num js-line-number" data-line-number="66"></td>
        <td id="LC66" class="blob-code js-file-line">    <span class="cm">/* ... and normalize them */</span></td>
      </tr>
      <tr>
        <td id="L67" class="blob-num js-line-number" data-line-number="67"></td>
        <td id="LC67" class="blob-code js-file-line">    <span class="kt">float</span> <span class="n">ss_X</span>      <span class="o">=</span> <span class="n">sumSquared</span><span class="p">(</span> <span class="n">X0</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L68" class="blob-num js-line-number" data-line-number="68"></td>
        <td id="LC68" class="blob-code js-file-line">    <span class="kt">float</span> <span class="n">norm_X</span>    <span class="o">=</span> <span class="n">sqrt</span><span class="p">(</span> <span class="n">ss_X</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L69" class="blob-num js-line-number" data-line-number="69"></td>
        <td id="LC69" class="blob-code js-file-line">    <span class="n">X0</span>              <span class="o">/=</span> <span class="n">norm_X</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L70" class="blob-num js-line-number" data-line-number="70"></td>
        <td id="LC70" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L71" class="blob-num js-line-number" data-line-number="71"></td>
        <td id="LC71" class="blob-code js-file-line">    <span class="kt">float</span> <span class="n">ss_Y</span>      <span class="o">=</span> <span class="n">sumSquared</span><span class="p">(</span> <span class="n">Y0</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L72" class="blob-num js-line-number" data-line-number="72"></td>
        <td id="LC72" class="blob-code js-file-line">    <span class="kt">float</span> <span class="n">norm_Y</span>    <span class="o">=</span> <span class="n">sqrt</span><span class="p">(</span> <span class="n">ss_Y</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L73" class="blob-num js-line-number" data-line-number="73"></td>
        <td id="LC73" class="blob-code js-file-line">    <span class="n">Y0</span>              <span class="o">/=</span> <span class="n">norm_Y</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L74" class="blob-num js-line-number" data-line-number="74"></td>
        <td id="LC74" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L75" class="blob-num js-line-number" data-line-number="75"></td>
        <td id="LC75" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L76" class="blob-num js-line-number" data-line-number="76"></td>
        <td id="LC76" class="blob-code js-file-line">    <span class="cm">/* Pad with zeros is Y has less points than X */</span></td>
      </tr>
      <tr>
        <td id="L77" class="blob-num js-line-number" data-line-number="77"></td>
        <td id="LC77" class="blob-code js-file-line">    <span class="k">if</span><span class="p">(</span> <span class="n">Y</span><span class="p">.</span><span class="n">rows</span> <span class="o">&lt;</span> <span class="n">X</span><span class="p">.</span><span class="n">rows</span> <span class="p">)</span></td>
      </tr>
      <tr>
        <td id="L78" class="blob-num js-line-number" data-line-number="78"></td>
        <td id="LC78" class="blob-code js-file-line">        <span class="n">vconcat</span><span class="p">(</span> <span class="n">Y0</span><span class="p">,</span> <span class="n">Mat</span><span class="o">::</span><span class="n">zeros</span><span class="p">(</span> <span class="n">X</span><span class="p">.</span><span class="n">rows</span> <span class="o">-</span> <span class="n">Y</span><span class="p">.</span><span class="n">rows</span><span class="p">,</span> <span class="mi">1</span><span class="p">,</span> <span class="n">Y</span><span class="p">.</span><span class="n">type</span><span class="p">()),</span> <span class="n">Y0</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L79" class="blob-num js-line-number" data-line-number="79"></td>
        <td id="LC79" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L80" class="blob-num js-line-number" data-line-number="80"></td>
        <td id="LC80" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L81" class="blob-num js-line-number" data-line-number="81"></td>
        <td id="LC81" class="blob-code js-file-line">    <span class="cm">/* Perform SVD */</span></td>
      </tr>
      <tr>
        <td id="L82" class="blob-num js-line-number" data-line-number="82"></td>
        <td id="LC82" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">A</span> <span class="o">=</span> <span class="n">X0</span><span class="p">.</span><span class="n">reshape</span><span class="p">(</span><span class="mi">1</span><span class="p">).</span><span class="n">t</span><span class="p">()</span> <span class="o">*</span> <span class="n">Y0</span><span class="p">.</span><span class="n">reshape</span><span class="p">(</span><span class="mi">1</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L83" class="blob-num js-line-number" data-line-number="83"></td>
        <td id="LC83" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">U</span><span class="p">,</span> <span class="n">s</span><span class="p">,</span> <span class="n">Vt</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L84" class="blob-num js-line-number" data-line-number="84"></td>
        <td id="LC84" class="blob-code js-file-line">    <span class="n">SVDecomp</span><span class="p">(</span> <span class="n">A</span><span class="p">,</span> <span class="n">s</span><span class="p">,</span> <span class="n">U</span><span class="p">,</span> <span class="n">Vt</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L85" class="blob-num js-line-number" data-line-number="85"></td>
        <td id="LC85" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L86" class="blob-num js-line-number" data-line-number="86"></td>
        <td id="LC86" class="blob-code js-file-line">    <span class="cm">/* Since in USV, U and V represents the rotation, and S is the scaling... */</span></td>
      </tr>
      <tr>
        <td id="L87" class="blob-num js-line-number" data-line-number="87"></td>
        <td id="LC87" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">V</span>           <span class="o">=</span> <span class="n">Vt</span><span class="p">.</span><span class="n">t</span><span class="p">();</span></td>
      </tr>
      <tr>
        <td id="L88" class="blob-num js-line-number" data-line-number="88"></td>
        <td id="LC88" class="blob-code js-file-line">    <span class="k">this</span><span class="o">-&gt;</span><span class="n">rotation</span>  <span class="o">=</span> <span class="n">V</span> <span class="o">*</span> <span class="n">U</span><span class="p">.</span><span class="n">t</span><span class="p">();</span></td>
      </tr>
      <tr>
        <td id="L89" class="blob-num js-line-number" data-line-number="89"></td>
        <td id="LC89" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L90" class="blob-num js-line-number" data-line-number="90"></td>
        <td id="LC90" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L91" class="blob-num js-line-number" data-line-number="91"></td>
        <td id="LC91" class="blob-code js-file-line">    <span class="k">if</span><span class="p">(</span> <span class="o">!</span><span class="n">bestReflection</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L92" class="blob-num js-line-number" data-line-number="92"></td>
        <td id="LC92" class="blob-code js-file-line">        <span class="kt">bool</span> <span class="n">have_reflection</span> <span class="o">=</span> <span class="n">determinant</span><span class="p">(</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">rotation</span> <span class="p">)</span> <span class="o">&lt;</span> <span class="mi">0</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L93" class="blob-num js-line-number" data-line-number="93"></td>
        <td id="LC93" class="blob-code js-file-line">        <span class="k">if</span><span class="p">(</span> <span class="n">bestReflection</span> <span class="o">!=</span> <span class="n">have_reflection</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L94" class="blob-num js-line-number" data-line-number="94"></td>
        <td id="LC94" class="blob-code js-file-line">            <span class="n">V</span><span class="p">.</span><span class="n">colRange</span><span class="p">(</span> <span class="n">V</span><span class="p">.</span><span class="n">cols</span><span class="o">-</span><span class="mi">1</span><span class="p">,</span> <span class="n">V</span><span class="p">.</span><span class="n">cols</span><span class="p">)</span> <span class="o">*=</span> <span class="o">-</span><span class="mi">1</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L95" class="blob-num js-line-number" data-line-number="95"></td>
        <td id="LC95" class="blob-code js-file-line">            <span class="n">s</span><span class="p">.</span><span class="n">rowRange</span><span class="p">(</span> <span class="n">s</span><span class="p">.</span><span class="n">rows</span><span class="o">-</span><span class="mi">1</span><span class="p">,</span> <span class="n">s</span><span class="p">.</span><span class="n">rows</span><span class="p">)</span> <span class="o">*=</span> <span class="o">-</span><span class="mi">1</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L96" class="blob-num js-line-number" data-line-number="96"></td>
        <td id="LC96" class="blob-code js-file-line">            <span class="k">this</span><span class="o">-&gt;</span><span class="n">rotation</span> <span class="o">=</span> <span class="n">V</span> <span class="o">*</span> <span class="n">U</span><span class="p">.</span><span class="n">t</span><span class="p">();</span></td>
      </tr>
      <tr>
        <td id="L97" class="blob-num js-line-number" data-line-number="97"></td>
        <td id="LC97" class="blob-code js-file-line">        <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L98" class="blob-num js-line-number" data-line-number="98"></td>
        <td id="LC98" class="blob-code js-file-line">    <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L99" class="blob-num js-line-number" data-line-number="99"></td>
        <td id="LC99" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L100" class="blob-num js-line-number" data-line-number="100"></td>
        <td id="LC100" class="blob-code js-file-line">    <span class="cm">/* Rotate Y0 first */</span></td>
      </tr>
      <tr>
        <td id="L101" class="blob-num js-line-number" data-line-number="101"></td>
        <td id="LC101" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">rotated_Y0</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L102" class="blob-num js-line-number" data-line-number="102"></td>
        <td id="LC102" class="blob-code js-file-line">    <span class="n">cv</span><span class="o">::</span><span class="n">transform</span><span class="p">(</span> <span class="n">Y0</span><span class="p">,</span> <span class="n">rotated_Y0</span><span class="p">,</span> <span class="n">rotation</span><span class="p">.</span><span class="n">t</span><span class="p">()</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L103" class="blob-num js-line-number" data-line-number="103"></td>
        <td id="LC103" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L104" class="blob-num js-line-number" data-line-number="104"></td>
        <td id="LC104" class="blob-code js-file-line">    <span class="cm">/* Trace of eigenvalues is basically the scale */</span></td>
      </tr>
      <tr>
        <td id="L105" class="blob-num js-line-number" data-line-number="105"></td>
        <td id="LC105" class="blob-code js-file-line">    <span class="kt">float</span> <span class="n">trace_TA</span> <span class="o">=</span> <span class="n">sum</span><span class="p">(</span> <span class="n">s</span> <span class="p">)[</span><span class="mi">0</span><span class="p">];</span></td>
      </tr>
      <tr>
        <td id="L106" class="blob-num js-line-number" data-line-number="106"></td>
        <td id="LC106" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L107" class="blob-num js-line-number" data-line-number="107"></td>
        <td id="LC107" class="blob-code js-file-line">    <span class="k">if</span><span class="p">(</span> <span class="n">scaling</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L108" class="blob-num js-line-number" data-line-number="108"></td>
        <td id="LC108" class="blob-code js-file-line">        <span class="n">scale</span>   <span class="o">=</span> <span class="n">trace_TA</span> <span class="o">*</span> <span class="n">norm_X</span> <span class="o">/</span> <span class="n">norm_Y</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L109" class="blob-num js-line-number" data-line-number="109"></td>
        <td id="LC109" class="blob-code js-file-line">        <span class="n">error</span>   <span class="o">=</span> <span class="mi">1</span> <span class="o">-</span> <span class="n">trace_TA</span> <span class="o">*</span> <span class="n">trace_TA</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L110" class="blob-num js-line-number" data-line-number="110"></td>
        <td id="LC110" class="blob-code js-file-line">        <span class="n">Yprime</span>  <span class="o">=</span> <span class="n">norm_X</span> <span class="o">*</span> <span class="n">trace_TA</span> <span class="o">*</span> <span class="n">rotated_Y0</span> <span class="o">+</span> <span class="n">mu_x</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L111" class="blob-num js-line-number" data-line-number="111"></td>
        <td id="LC111" class="blob-code js-file-line">    <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L112" class="blob-num js-line-number" data-line-number="112"></td>
        <td id="LC112" class="blob-code js-file-line">    <span class="k">else</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L113" class="blob-num js-line-number" data-line-number="113"></td>
        <td id="LC113" class="blob-code js-file-line">        <span class="n">error</span>   <span class="o">=</span> <span class="mi">1</span> <span class="o">+</span> <span class="n">ss_Y</span> <span class="o">/</span> <span class="n">ss_X</span> <span class="o">-</span> <span class="mi">2</span> <span class="o">*</span> <span class="n">trace_TA</span> <span class="o">*</span> <span class="n">norm_X</span> <span class="o">/</span> <span class="n">norm_Y</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L114" class="blob-num js-line-number" data-line-number="114"></td>
        <td id="LC114" class="blob-code js-file-line">        <span class="n">Yprime</span>  <span class="o">=</span> <span class="n">norm_Y</span> <span class="o">*</span> <span class="n">rotated_Y0</span> <span class="o">+</span> <span class="n">mu_x</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L115" class="blob-num js-line-number" data-line-number="115"></td>
        <td id="LC115" class="blob-code js-file-line">    <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L116" class="blob-num js-line-number" data-line-number="116"></td>
        <td id="LC116" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L117" class="blob-num js-line-number" data-line-number="117"></td>
        <td id="LC117" class="blob-code js-file-line">    <span class="k">if</span><span class="p">(</span> <span class="n">Y</span><span class="p">.</span><span class="n">rows</span> <span class="o">&lt;</span> <span class="n">X</span><span class="p">.</span><span class="n">rows</span> <span class="p">)</span></td>
      </tr>
      <tr>
        <td id="L118" class="blob-num js-line-number" data-line-number="118"></td>
        <td id="LC118" class="blob-code js-file-line">        <span class="n">rotation</span> <span class="o">=</span> <span class="n">rotation</span><span class="p">.</span><span class="n">rowRange</span><span class="p">(</span><span class="mi">0</span><span class="p">,</span> <span class="n">Y</span><span class="p">.</span><span class="n">rows</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L119" class="blob-num js-line-number" data-line-number="119"></td>
        <td id="LC119" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L120" class="blob-num js-line-number" data-line-number="120"></td>
        <td id="LC120" class="blob-code js-file-line">    <span class="n">translation</span> <span class="o">=</span> <span class="n">Mat</span><span class="p">(</span><span class="mi">1</span><span class="p">,</span> <span class="mi">1</span><span class="p">,</span> <span class="n">CV_32FC2</span><span class="p">,</span> <span class="n">mu_x</span><span class="p">).</span><span class="n">reshape</span><span class="p">(</span><span class="mi">1</span><span class="p">)</span> <span class="o">-</span> <span class="n">scale</span> <span class="o">*</span> <span class="n">Mat</span><span class="p">(</span><span class="mi">1</span><span class="p">,</span> <span class="mi">1</span><span class="p">,</span> <span class="n">CV_32FC2</span><span class="p">,</span> <span class="n">mu_y</span><span class="p">).</span><span class="n">reshape</span><span class="p">(</span><span class="mi">1</span><span class="p">)</span> <span class="o">*</span> <span class="n">rotation</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L121" class="blob-num js-line-number" data-line-number="121"></td>
        <td id="LC121" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L122" class="blob-num js-line-number" data-line-number="122"></td>
        <td id="LC122" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">error</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L123" class="blob-num js-line-number" data-line-number="123"></td>
        <td id="LC123" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L124" class="blob-num js-line-number" data-line-number="124"></td>
        <td id="LC124" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L125" class="blob-num js-line-number" data-line-number="125"></td>
        <td id="LC125" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L126" class="blob-num js-line-number" data-line-number="126"></td>
        <td id="LC126" class="blob-code js-file-line"><span class="cm"> * Recenter each matrix / set of points, by subtracting them with the mean / centroid</span></td>
      </tr>
      <tr>
        <td id="L127" class="blob-num js-line-number" data-line-number="127"></td>
        <td id="LC127" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L128" class="blob-num js-line-number" data-line-number="128"></td>
        <td id="LC128" class="blob-code js-file-line"><span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">recenter</span><span class="p">(</span> <span class="k">const</span> <span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;&amp;</span> <span class="n">X</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L129" class="blob-num js-line-number" data-line-number="129"></td>
        <td id="LC129" class="blob-code js-file-line">    <span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L130" class="blob-num js-line-number" data-line-number="130"></td>
        <td id="LC130" class="blob-code js-file-line">    <span class="k">for</span><span class="p">(</span> <span class="n">Mat</span> <span class="nl">x</span> <span class="p">:</span> <span class="n">X</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L131" class="blob-num js-line-number" data-line-number="131"></td>
        <td id="LC131" class="blob-code js-file-line">        <span class="n">Scalar</span> <span class="n">mu_x</span> <span class="o">=</span> <span class="n">cv</span><span class="o">::</span><span class="n">mean</span><span class="p">(</span><span class="n">x</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L132" class="blob-num js-line-number" data-line-number="132"></td>
        <td id="LC132" class="blob-code js-file-line">        <span class="n">Mat</span> <span class="n">temp</span>    <span class="o">=</span> <span class="n">x</span> <span class="o">-</span> <span class="n">Mat</span><span class="p">(</span><span class="n">x</span><span class="p">.</span><span class="n">size</span><span class="p">(),</span> <span class="n">x</span><span class="p">.</span><span class="n">type</span><span class="p">(),</span> <span class="n">mu_x</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L133" class="blob-num js-line-number" data-line-number="133"></td>
        <td id="LC133" class="blob-code js-file-line">        <span class="n">result</span><span class="p">.</span><span class="n">push_back</span><span class="p">(</span> <span class="n">temp</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L134" class="blob-num js-line-number" data-line-number="134"></td>
        <td id="LC134" class="blob-code js-file-line">    <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L135" class="blob-num js-line-number" data-line-number="135"></td>
        <td id="LC135" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L136" class="blob-num js-line-number" data-line-number="136"></td>
        <td id="LC136" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L137" class="blob-num js-line-number" data-line-number="137"></td>
        <td id="LC137" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L138" class="blob-num js-line-number" data-line-number="138"></td>
        <td id="LC138" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L139" class="blob-num js-line-number" data-line-number="139"></td>
        <td id="LC139" class="blob-code js-file-line"><span class="cm"> * Normalize each set of points</span></td>
      </tr>
      <tr>
        <td id="L140" class="blob-num js-line-number" data-line-number="140"></td>
        <td id="LC140" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L141" class="blob-num js-line-number" data-line-number="141"></td>
        <td id="LC141" class="blob-code js-file-line"><span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">normalize</span><span class="p">(</span> <span class="k">const</span> <span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;&amp;</span> <span class="n">X</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L142" class="blob-num js-line-number" data-line-number="142"></td>
        <td id="LC142" class="blob-code js-file-line">    <span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L143" class="blob-num js-line-number" data-line-number="143"></td>
        <td id="LC143" class="blob-code js-file-line">    <span class="k">for</span><span class="p">(</span> <span class="n">Mat</span> <span class="nl">x</span> <span class="p">:</span> <span class="n">X</span> <span class="p">)</span></td>
      </tr>
      <tr>
        <td id="L144" class="blob-num js-line-number" data-line-number="144"></td>
        <td id="LC144" class="blob-code js-file-line">        <span class="n">result</span><span class="p">.</span><span class="n">push_back</span><span class="p">(</span> <span class="n">x</span> <span class="o">/</span> <span class="n">cv</span><span class="o">::</span><span class="n">norm</span><span class="p">(</span> <span class="n">x</span> <span class="p">)</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L145" class="blob-num js-line-number" data-line-number="145"></td>
        <td id="LC145" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L146" class="blob-num js-line-number" data-line-number="146"></td>
        <td id="LC146" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L147" class="blob-num js-line-number" data-line-number="147"></td>
        <td id="LC147" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L148" class="blob-num js-line-number" data-line-number="148"></td>
        <td id="LC148" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L149" class="blob-num js-line-number" data-line-number="149"></td>
        <td id="LC149" class="blob-code js-file-line"><span class="cm"> * Perform rotation alignment between a set of points / shapes and a chosen mean shape</span></td>
      </tr>
      <tr>
        <td id="L150" class="blob-num js-line-number" data-line-number="150"></td>
        <td id="LC150" class="blob-code js-file-line"><span class="cm"> * Points and mean shape should already be centered and normalize prior to the call of this function</span></td>
      </tr>
      <tr>
        <td id="L151" class="blob-num js-line-number" data-line-number="151"></td>
        <td id="LC151" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L152" class="blob-num js-line-number" data-line-number="152"></td>
        <td id="LC152" class="blob-code js-file-line"><span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">align</span><span class="p">(</span> <span class="k">const</span> <span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;&amp;</span> <span class="n">X</span><span class="p">,</span> <span class="n">Mat</span><span class="o">&amp;</span> <span class="n">mean_shape</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L153" class="blob-num js-line-number" data-line-number="153"></td>
        <td id="LC153" class="blob-code js-file-line">    <span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L154" class="blob-num js-line-number" data-line-number="154"></td>
        <td id="LC154" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">w</span><span class="p">,</span> <span class="n">u</span><span class="p">,</span> <span class="n">vt</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L155" class="blob-num js-line-number" data-line-number="155"></td>
        <td id="LC155" class="blob-code js-file-line">    <span class="k">for</span><span class="p">(</span> <span class="n">Mat</span> <span class="nl">x</span> <span class="p">:</span> <span class="n">X</span> <span class="p">){</span></td>
      </tr>
      <tr>
        <td id="L156" class="blob-num js-line-number" data-line-number="156"></td>
        <td id="LC156" class="blob-code js-file-line">        <span class="n">SVDecomp</span><span class="p">(</span> <span class="n">mean_shape</span><span class="p">.</span><span class="n">reshape</span><span class="p">(</span><span class="mi">1</span><span class="p">).</span><span class="n">t</span><span class="p">()</span> <span class="o">*</span> <span class="n">x</span><span class="p">.</span><span class="n">reshape</span><span class="p">(</span><span class="mi">1</span><span class="p">)</span> <span class="p">,</span> <span class="n">w</span><span class="p">,</span> <span class="n">u</span><span class="p">,</span> <span class="n">vt</span><span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L157" class="blob-num js-line-number" data-line-number="157"></td>
        <td id="LC157" class="blob-code js-file-line">        <span class="n">result</span><span class="p">.</span><span class="n">push_back</span><span class="p">(</span> <span class="p">(</span><span class="n">x</span><span class="p">.</span><span class="n">reshape</span><span class="p">(</span><span class="mi">1</span><span class="p">)</span> <span class="o">*</span> <span class="n">vt</span><span class="p">.</span><span class="n">t</span><span class="p">())</span> <span class="o">*</span> <span class="n">u</span><span class="p">.</span><span class="n">t</span><span class="p">()</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L158" class="blob-num js-line-number" data-line-number="158"></td>
        <td id="LC158" class="blob-code js-file-line">    <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L159" class="blob-num js-line-number" data-line-number="159"></td>
        <td id="LC159" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L160" class="blob-num js-line-number" data-line-number="160"></td>
        <td id="LC160" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L161" class="blob-num js-line-number" data-line-number="161"></td>
        <td id="LC161" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L162" class="blob-num js-line-number" data-line-number="162"></td>
        <td id="LC162" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L163" class="blob-num js-line-number" data-line-number="163"></td>
        <td id="LC163" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L164" class="blob-num js-line-number" data-line-number="164"></td>
        <td id="LC164" class="blob-code js-file-line"><span class="cm"> * Wrapper for vector&lt;vector&lt;Point2f&gt;&gt;</span></td>
      </tr>
      <tr>
        <td id="L165" class="blob-num js-line-number" data-line-number="165"></td>
        <td id="LC165" class="blob-code js-file-line"><span class="cm"> */</span></td>
      </tr>
      <tr>
        <td id="L166" class="blob-num js-line-number" data-line-number="166"></td>
        <td id="LC166" class="blob-code js-file-line"><span class="n">vector</span><span class="o">&lt;</span><span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;&gt;</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">generalizedProcrustes</span><span class="p">(</span> <span class="n">vector</span><span class="o">&lt;</span><span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;&gt;&amp;</span> <span class="n">X</span><span class="p">,</span> <span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;&amp;</span> <span class="n">mean_shape</span><span class="p">,</span> <span class="k">const</span> <span class="kt">int</span> <span class="n">itol</span><span class="p">,</span> <span class="k">const</span> <span class="kt">float</span> <span class="n">ftol</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L167" class="blob-num js-line-number" data-line-number="167"></td>
        <td id="LC167" class="blob-code js-file-line">    <span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">temp</span><span class="p">(</span><span class="n">X</span><span class="p">.</span><span class="n">size</span><span class="p">());</span></td>
      </tr>
      <tr>
        <td id="L168" class="blob-num js-line-number" data-line-number="168"></td>
        <td id="LC168" class="blob-code js-file-line">    <span class="k">for</span><span class="p">(</span> <span class="kt">int</span> <span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">temp</span><span class="p">.</span><span class="n">size</span><span class="p">();</span> <span class="n">i</span><span class="o">++</span> <span class="p">)</span></td>
      </tr>
      <tr>
        <td id="L169" class="blob-num js-line-number" data-line-number="169"></td>
        <td id="LC169" class="blob-code js-file-line">        <span class="n">temp</span><span class="p">[</span><span class="n">i</span><span class="p">]</span> <span class="o">=</span> <span class="n">Mat</span><span class="p">(</span> <span class="n">X</span><span class="p">[</span><span class="n">i</span><span class="p">]</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L170" class="blob-num js-line-number" data-line-number="170"></td>
        <td id="LC170" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L171" class="blob-num js-line-number" data-line-number="171"></td>
        <td id="LC171" class="blob-code js-file-line">    <span class="n">Mat</span> <span class="n">mean_shape_mat</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L172" class="blob-num js-line-number" data-line-number="172"></td>
        <td id="LC172" class="blob-code js-file-line">    <span class="n">temp</span> <span class="o">=</span> <span class="n">generalizedProcrustes</span><span class="p">(</span> <span class="n">temp</span><span class="p">,</span> <span class="n">mean_shape_mat</span><span class="p">,</span> <span class="n">itol</span><span class="p">,</span> <span class="n">ftol</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L173" class="blob-num js-line-number" data-line-number="173"></td>
        <td id="LC173" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L174" class="blob-num js-line-number" data-line-number="174"></td>
        <td id="LC174" class="blob-code js-file-line">    <span class="cm">/* Copy the mean shape back as vector of Point2f */</span></td>
      </tr>
      <tr>
        <td id="L175" class="blob-num js-line-number" data-line-number="175"></td>
        <td id="LC175" class="blob-code js-file-line">    <span class="n">mean_shape</span><span class="p">.</span><span class="n">clear</span><span class="p">();</span></td>
      </tr>
      <tr>
        <td id="L176" class="blob-num js-line-number" data-line-number="176"></td>
        <td id="LC176" class="blob-code js-file-line">    <span class="n">mean_shape_mat</span><span class="p">.</span><span class="n">reshape</span><span class="p">(</span><span class="mi">2</span><span class="p">).</span><span class="n">copyTo</span><span class="p">(</span> <span class="n">mean_shape</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L177" class="blob-num js-line-number" data-line-number="177"></td>
        <td id="LC177" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L178" class="blob-num js-line-number" data-line-number="178"></td>
        <td id="LC178" class="blob-code js-file-line">    <span class="cm">/* Copy the transformed X as vector of vector of Point2f, very circuitous way of doing thing */</span></td>
      </tr>
      <tr>
        <td id="L179" class="blob-num js-line-number" data-line-number="179"></td>
        <td id="LC179" class="blob-code js-file-line">    <span class="n">vector</span><span class="o">&lt;</span><span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;&gt;</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L180" class="blob-num js-line-number" data-line-number="180"></td>
        <td id="LC180" class="blob-code js-file-line">    <span class="k">for</span><span class="p">(</span> <span class="n">Mat</span> <span class="nl">mat</span> <span class="p">:</span> <span class="n">temp</span> <span class="p">){</span></td>
      </tr>
      <tr>
        <td id="L181" class="blob-num js-line-number" data-line-number="181"></td>
        <td id="LC181" class="blob-code js-file-line">        <span class="n">vector</span><span class="o">&lt;</span><span class="n">Point2f</span><span class="o">&gt;</span> <span class="n">vec</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L182" class="blob-num js-line-number" data-line-number="182"></td>
        <td id="LC182" class="blob-code js-file-line">        <span class="n">mat</span><span class="p">.</span><span class="n">reshape</span><span class="p">(</span><span class="mi">2</span><span class="p">).</span><span class="n">copyTo</span><span class="p">(</span> <span class="n">vec</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L183" class="blob-num js-line-number" data-line-number="183"></td>
        <td id="LC183" class="blob-code js-file-line">        <span class="n">result</span><span class="p">.</span><span class="n">push_back</span><span class="p">(</span> <span class="n">vec</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L184" class="blob-num js-line-number" data-line-number="184"></td>
        <td id="LC184" class="blob-code js-file-line">    <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L185" class="blob-num js-line-number" data-line-number="185"></td>
        <td id="LC185" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L186" class="blob-num js-line-number" data-line-number="186"></td>
        <td id="LC186" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">result</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L187" class="blob-num js-line-number" data-line-number="187"></td>
        <td id="LC187" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L188" class="blob-num js-line-number" data-line-number="188"></td>
        <td id="LC188" class="blob-code js-file-line">
</td>
      </tr>
      <tr>
        <td id="L189" class="blob-num js-line-number" data-line-number="189"></td>
        <td id="LC189" class="blob-code js-file-line"><span class="cm">/**</span></td>
      </tr>
      <tr>
        <td id="L190" class="blob-num js-line-number" data-line-number="190"></td>
        <td id="LC190" class="blob-code js-file-line"><span class="cm"> * Perform a generalized Procrustes analysis to find the mean shape</span></td>
      </tr>
      <tr>
        <td id="L191" class="blob-num js-line-number" data-line-number="191"></td>
        <td id="LC191" class="blob-code js-file-line"><span class="cm"> **/</span></td>
      </tr>
      <tr>
        <td id="L192" class="blob-num js-line-number" data-line-number="192"></td>
        <td id="LC192" class="blob-code js-file-line"><span class="n">vector</span><span class="o">&lt;</span><span class="n">Mat</span><span class="o">&gt;</span> <span class="n">Procrustes</span><span class="o">::</span><span class="n">generalizedProcrustes</span><span class="p">(</span> <span class="n">std</span><span class="o">::</span><span class="n">vector</span><span class="o">&lt;</span><span class="n">cv</span><span class="o">::</span><span class="n">Mat</span><span class="o">&gt;&amp;</span> <span class="n">X</span><span class="p">,</span> <span class="n">Mat</span><span class="o">&amp;</span> <span class="n">mean_shape</span><span class="p">,</span> <span class="k">const</span> <span class="kt">int</span> <span class="n">itol</span><span class="p">,</span> <span class="k">const</span> <span class="kt">float</span> <span class="n">ftol</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L193" class="blob-num js-line-number" data-line-number="193"></td>
        <td id="LC193" class="blob-code js-file-line">    <span class="cm">/* Arbitrarily choose the first set of points as our mean shape */</span></td>
      </tr>
      <tr>
        <td id="L194" class="blob-num js-line-number" data-line-number="194"></td>
        <td id="LC194" class="blob-code js-file-line">    <span class="n">mean_shape</span> <span class="o">=</span> <span class="n">X</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">reshape</span><span class="p">(</span> <span class="mi">1</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L195" class="blob-num js-line-number" data-line-number="195"></td>
        <td id="LC195" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L196" class="blob-num js-line-number" data-line-number="196"></td>
        <td id="LC196" class="blob-code js-file-line">    <span class="kt">int</span> <span class="n">counter</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L197" class="blob-num js-line-number" data-line-number="197"></td>
        <td id="LC197" class="blob-code js-file-line">    <span class="k">while</span><span class="p">(</span> <span class="nb">true</span> <span class="p">)</span> <span class="p">{</span></td>
      </tr>
      <tr>
        <td id="L198" class="blob-num js-line-number" data-line-number="198"></td>
        <td id="LC198" class="blob-code js-file-line">        <span class="cm">/* recenter, normalize, align */</span></td>
      </tr>
      <tr>
        <td id="L199" class="blob-num js-line-number" data-line-number="199"></td>
        <td id="LC199" class="blob-code js-file-line">        <span class="n">X</span> <span class="o">=</span> <span class="n">recenter</span><span class="p">(</span> <span class="n">X</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L200" class="blob-num js-line-number" data-line-number="200"></td>
        <td id="LC200" class="blob-code js-file-line">        <span class="n">X</span> <span class="o">=</span> <span class="n">normalize</span><span class="p">(</span> <span class="n">X</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L201" class="blob-num js-line-number" data-line-number="201"></td>
        <td id="LC201" class="blob-code js-file-line">        <span class="n">X</span> <span class="o">=</span> <span class="n">align</span><span class="p">(</span> <span class="n">X</span><span class="p">,</span> <span class="n">mean_shape</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L202" class="blob-num js-line-number" data-line-number="202"></td>
        <td id="LC202" class="blob-code js-file-line">        </td>
      </tr>
      <tr>
        <td id="L203" class="blob-num js-line-number" data-line-number="203"></td>
        <td id="LC203" class="blob-code js-file-line">        </td>
      </tr>
      <tr>
        <td id="L204" class="blob-num js-line-number" data-line-number="204"></td>
        <td id="LC204" class="blob-code js-file-line">        <span class="cm">/* Find a new mean shape from all the set of points */</span></td>
      </tr>
      <tr>
        <td id="L205" class="blob-num js-line-number" data-line-number="205"></td>
        <td id="LC205" class="blob-code js-file-line">        <span class="n">Mat</span> <span class="n">new_mean</span> <span class="o">=</span> <span class="n">Mat</span><span class="o">::</span><span class="n">zeros</span><span class="p">(</span> <span class="n">mean_shape</span><span class="p">.</span><span class="n">size</span><span class="p">(),</span> <span class="n">mean_shape</span><span class="p">.</span><span class="n">type</span><span class="p">()</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L206" class="blob-num js-line-number" data-line-number="206"></td>
        <td id="LC206" class="blob-code js-file-line">        <span class="n">accumulate</span><span class="p">(</span> <span class="n">X</span><span class="p">.</span><span class="n">begin</span><span class="p">(),</span> <span class="n">X</span><span class="p">.</span><span class="n">end</span><span class="p">(),</span> <span class="n">new_mean</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L207" class="blob-num js-line-number" data-line-number="207"></td>
        <td id="LC207" class="blob-code js-file-line">        </td>
      </tr>
      <tr>
        <td id="L208" class="blob-num js-line-number" data-line-number="208"></td>
        <td id="LC208" class="blob-code js-file-line">        <span class="n">new_mean</span> <span class="o">=</span> <span class="n">new_mean</span> <span class="o">/</span> <span class="n">X</span><span class="p">.</span><span class="n">size</span><span class="p">();</span></td>
      </tr>
      <tr>
        <td id="L209" class="blob-num js-line-number" data-line-number="209"></td>
        <td id="LC209" class="blob-code js-file-line">        <span class="n">new_mean</span> <span class="o">=</span> <span class="n">new_mean</span> <span class="o">/</span> <span class="n">norm</span><span class="p">(</span> <span class="n">new_mean</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L210" class="blob-num js-line-number" data-line-number="210"></td>
        <td id="LC210" class="blob-code js-file-line">        </td>
      </tr>
      <tr>
        <td id="L211" class="blob-num js-line-number" data-line-number="211"></td>
        <td id="LC211" class="blob-code js-file-line">        <span class="cm">/* Perform the loop until convergence */</span></td>
      </tr>
      <tr>
        <td id="L212" class="blob-num js-line-number" data-line-number="212"></td>
        <td id="LC212" class="blob-code js-file-line">        <span class="kt">float</span> <span class="n">diff</span> <span class="o">=</span> <span class="n">norm</span><span class="p">(</span> <span class="n">new_mean</span><span class="p">,</span> <span class="n">mean_shape</span> <span class="p">);</span></td>
      </tr>
      <tr>
        <td id="L213" class="blob-num js-line-number" data-line-number="213"></td>
        <td id="LC213" class="blob-code js-file-line">        <span class="k">if</span><span class="p">(</span> <span class="n">counter</span><span class="o">++</span> <span class="o">&gt;</span> <span class="n">itol</span> <span class="o">||</span> <span class="n">diff</span> <span class="o">&lt;=</span> <span class="n">ftol</span>  <span class="p">)</span></td>
      </tr>
      <tr>
        <td id="L214" class="blob-num js-line-number" data-line-number="214"></td>
        <td id="LC214" class="blob-code js-file-line">            <span class="k">break</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L215" class="blob-num js-line-number" data-line-number="215"></td>
        <td id="LC215" class="blob-code js-file-line">        </td>
      </tr>
      <tr>
        <td id="L216" class="blob-num js-line-number" data-line-number="216"></td>
        <td id="LC216" class="blob-code js-file-line">        <span class="n">mean_shape</span> <span class="o">=</span> <span class="n">new_mean</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L217" class="blob-num js-line-number" data-line-number="217"></td>
        <td id="LC217" class="blob-code js-file-line">    <span class="p">}</span></td>
      </tr>
      <tr>
        <td id="L218" class="blob-num js-line-number" data-line-number="218"></td>
        <td id="LC218" class="blob-code js-file-line">    </td>
      </tr>
      <tr>
        <td id="L219" class="blob-num js-line-number" data-line-number="219"></td>
        <td id="LC219" class="blob-code js-file-line">    <span class="k">return</span> <span class="n">X</span><span class="p">;</span></td>
      </tr>
      <tr>
        <td id="L220" class="blob-num js-line-number" data-line-number="220"></td>
        <td id="LC220" class="blob-code js-file-line"><span class="p">}</span></td>
      </tr>
</table>

  </div>

  </div>
</div>

<a href="#jump-to-line" rel="facebox[.linejump]" data-hotkey="l" style="display:none">Jump to Line</a>
<div id="jump-to-line" style="display:none">
  <form accept-charset="UTF-8" class="js-jump-to-line-form">
    <input class="linejump-input js-jump-to-line-field" type="text" placeholder="Jump to line&hellip;" autofocus>
    <button type="submit" class="button">Go</button>
  </form>
</div>

        </div>

      </div><!-- /.repo-container -->
      <div class="modal-backdrop"></div>
    </div><!-- /.container -->
  </div><!-- /.site -->


    </div><!-- /.wrapper -->

      <div class="container">
  <div class="site-footer" role="contentinfo">
    <ul class="site-footer-links right">
      <li><a href="https://status.github.com/">Status</a></li>
      <li><a href="https://developer.github.com">API</a></li>
      <li><a href="http://training.github.com">Training</a></li>
      <li><a href="http://shop.github.com">Shop</a></li>
      <li><a href="/blog">Blog</a></li>
      <li><a href="/about">About</a></li>

    </ul>

    <a href="/" aria-label="Homepage">
      <span class="mega-octicon octicon-mark-github" title="GitHub"></span>
    </a>

    <ul class="site-footer-links">
      <li>&copy; 2014 <span title="0.03549s from github-fe124-cp1-prd.iad.github.net">GitHub</span>, Inc.</li>
        <li><a href="/site/terms">Terms</a></li>
        <li><a href="/site/privacy">Privacy</a></li>
        <li><a href="/security">Security</a></li>
        <li><a href="/contact">Contact</a></li>
    </ul>
  </div><!-- /.site-footer -->
</div><!-- /.container -->


    <div class="fullscreen-overlay js-fullscreen-overlay" id="fullscreen_overlay">
  <div class="fullscreen-container js-suggester-container">
    <div class="textarea-wrap">
      <textarea name="fullscreen-contents" id="fullscreen-contents" class="fullscreen-contents js-fullscreen-contents js-suggester-field" placeholder=""></textarea>
    </div>
  </div>
  <div class="fullscreen-sidebar">
    <a href="#" class="exit-fullscreen js-exit-fullscreen tooltipped tooltipped-w" aria-label="Exit Zen Mode">
      <span class="mega-octicon octicon-screen-normal"></span>
    </a>
    <a href="#" class="theme-switcher js-theme-switcher tooltipped tooltipped-w"
      aria-label="Switch themes">
      <span class="octicon octicon-color-mode"></span>
    </a>
  </div>
</div>



    <div id="ajax-error-message" class="flash flash-error">
      <span class="octicon octicon-alert"></span>
      <a href="#" class="octicon octicon-x flash-close js-ajax-error-dismiss" aria-label="Dismiss error"></a>
      Something went wrong with that request. Please try again.
    </div>


      <script crossorigin="anonymous" src="https://assets-cdn.github.com/assets/frameworks-95701e1c81a17a17316798e9675eea97f8071d2ab36b6cb9c28fff868abcdc07.js" type="text/javascript"></script>
      <script async="async" crossorigin="anonymous" src="https://assets-cdn.github.com/assets/github-232e6668807378a8b92f581067d7b48c422ab88fcb1c63e5404ed46b80aba19e.js" type="text/javascript"></script>
      
      
  </body>
</html>

