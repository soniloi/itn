#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LEAPYEAR_FREQUENCY 4
#define LEAPYEAR_FIRST_CORRECTOR 100 // Leap-years skipped every 100 years
#define LEAPYEAR_SECOND_CORRECTOR 1000 // Unless it's a millenial year
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define MONTHS_PER_YEAR 12
#define DAYS_PER_YEAR 365
#define MODE_ALPHABET_SIZE 26 // All mode-changes on this server taken to be lower-case single alphabetic characters
#define MODE_ALPHABET_OFFSET 97 // ASCII 'a'
#define ASCII_OFFSET_NUMERIC 0x30 // The difference between a number and a numeral e.g. 0 and '0'
#define RADIX_DEFAULT 10 // Used in things like string-to-int conversion
#define YEAR_ZERO 1970 // No reason not to go with the classic
#define CONTENT_FLAGS_J 1 // Content vector flag for join/part/quit/kill statistics
#define CONTENT_FLAGS_M 2 // Content vector flag for mode-change statistics
#define CONTENT_FLAGS_E 4 // Content vector flag for emotion statistics
#define CONTENT_FLAGS_F 8 // Content vector flag for feature statistics
#define CONTENT_FLAGS_W 16 // Content vector flag for word statistics
#define CONTENT_FLAGS_B 32 // Content vector flag for basic statistics
#define SPACE ' '
#define TAB '\t'
#define FILENAME_CONFIG "config.in"

typedef enum {SUCCEED, FAIL} successcode;

/* Keys for config maps */
#define KEY_CONF_BOT_CUTOFF "bot_cutoff"
#define KEY_CONF_BOT_LINECOUNT_CUTOFF "bot_linecount_cutoff"
#define KEY_CONF_BOT_PROXIMITY_0_SECONDS "bot_proximity_0_seconds"
#define KEY_CONF_BOT_PROXIMITY_0_VALUE "bot_proximity_0_value"
#define KEY_CONF_BOT_PROXIMITY_1_SECONDS "bot_proximity_1_seconds"
#define KEY_CONF_BOT_PROXIMITY_1_VALUE "bot_proximity_1_value"
#define KEY_CONF_BOT_PROXIMITY_2_SECONDS "bot_proximity_2_seconds"
#define KEY_CONF_BOT_PROXIMITY_2_VALUE "bot_proximity_2_value"
#define KEY_CONF_BOT_PROXIMITY_3_SECONDS "bot_proximity_3_seconds"
#define KEY_CONF_BOT_PROXIMITY_3_VALUE "bot_proximity_3_value"
#define KEY_CONF_BOT_TRIGGER_VALUE "bot_trigger_value"
#define KEY_CONF_DECIMAL_PLACES_TO_PRINT "decimal_places_to_print"
#define KEY_CONF_DICTATOR_INDICATOR_VALUE "dictator_indicator_value"
#define KEY_CONF_DICTATOR_SPEECH_MINIMUM "dictator_speech_minimum"
#define KEY_CONF_DICTATOR_SPEECH_VALUE "dictator_speech_value"
#define KEY_CONF_DICTATOR_SUGGESTOR_VALUE "dictator_suggestor_value"
#define KEY_CONF_HOUSE_INDICATOR_VALUE "house_indicator_value"
#define KEY_CONF_HOUSE_SUGGESTOR_VALUE "house_suggestor_value"
#define KEY_CONF_INFO_TOKENS_BEFORE_DATE "info_tokens_before_date"
#define KEY_CONF_LANG_MIN_FAVOURITE "lang_min_favourite"
#define KEY_CONF_LANG_MODIFIER_TOPUP "lang_modifier_topup"
#define KEY_CONF_MODE_MAX_CHANGES "mode_max_changes"
#define KEY_CONF_NICK_HASCLOSE "nick_hasclose"
#define KEY_CONF_NICK_HASOPEN "nick_hasopen"
#define KEY_CONF_NICK_LENGTH_MAX "nick_length_max"
#define KEY_CONF_OUT_WORDS_PER_LINE "out_words_per_line"
#define KEY_CONF_SIZE_LIST_LONG "size_list_long"
#define KEY_CONF_SIZE_LIST_MEDIUM "size_list_medium"
#define KEY_CONF_SIZE_LIST_SHORT "size_list_short"
#define KEY_CONF_SIZE_LIST_TOPICS "size_list_topics"
#define KEY_CONF_TIME_HAS_SECONDS "time_has_seconds"
#define KEY_CONF_TIME_HASCLOSE "time_hasclose"
#define KEY_CONF_TIME_HASOPEN "time_hasopen"
#define KEY_CONF_TIME_LENGTH "time_length"
#define KEY_CONF_WORDCOUNT_CUTOFF "wordcount_cutoff"
#define KEY_CONF_WORDCOUNT_EXCLUDED "wordcount_excluded"
#define KEY_CONF_WORDCOUNT_USERS "wordcount_users"

#define KEY_CONF_FILENAME_PATH_SEPARATOR "filename_path_separator"
#define KEY_CONF_HOSTMASK_CLOSE "hostmask_close"
#define KEY_CONF_HOSTMASK_OPEN "hostmask_open"
#define KEY_CONF_MARK_INFO "mark_info"
#define KEY_CONF_MARK_ME "mark_me"
#define KEY_CONF_NICK_CLOSE "nick_close"
#define KEY_CONF_NICK_OPEN "nick_open"
#define KEY_CONF_OUT_CLARIFY_CLOSE "out_clarify_close"
#define KEY_CONF_OUT_CLARIFY_OPEN "out_clarify_open"
#define KEY_CONF_OUT_DATE_SEPARATOR "out_date_separator"
#define KEY_CONF_OUT_LIST_CLOSE "out_list_close"
#define KEY_CONF_OUT_LIST_OPEN "out_list_open"
#define KEY_CONF_OUT_TIME_SEPARATOR "out_time_separator"
#define KEY_CONF_TIME_CLOSE "time_close"
#define KEY_CONF_TIME_OPEN "time_open"
#define KEY_CONF_TIME_SEPARATOR_INFO "time_separator_info"
#define KEY_CONF_TIME_SEPARATOR_OTHER "time_separator_other"

#define KEY_CONF_BOT_TRIGGERS "bot_triggers"
#define KEY_CONF_FILENAME_IGNORE "filename_ignore"
#define KEY_CONF_FILENAME_MERGE "filename_merge"
#define KEY_CONF_FILENAME_WORDS "filename_words"
#define KEY_CONF_FILENAME_OUT_DEFAULT "filename_out_default"
#define KEY_CONF_INFO_CHANGEBY "info_changeby"
#define KEY_CONF_INFO_DAYCHANGE "info_daychange"
#define KEY_CONF_INFO_JOIN "info_join"
#define KEY_CONF_INFO_KICK "info_kick"
#define KEY_CONF_INFO_KILL "info_kill"
#define KEY_CONF_INFO_LOG "info_log"
#define KEY_CONF_INFO_MODECHANGE "info_modechange"
#define KEY_CONF_INFO_NICKCHANGE "info_nickchange"
#define KEY_CONF_INFO_PART "info_part"
#define KEY_CONF_INFO_QUIT "info_quit"
#define KEY_CONF_INFO_TOPICCHANGE "info_topicchange"
#define KEY_CONF_INFO_TOPICCHANGE_TO "info_topicchange_to"
#define KEY_CONF_NICK_INVALID "nick_invalid"
#define KEY_CONF_OUT_SECTION_DIVIDER "out_section_divider"
#define KEY_CONF_WORD_VALID_DELIMITERS "word_valid_delimiters"
#define KEY_CONF_WORD_VALID_PUNCTUATION "word_valid_punctuation"

/* Keys for attribute maps */
#define KEY_ATTR_ACTIVITY "activity"
#define KEY_ATTR_ANGRY "angry"
#define KEY_ATTR_BOT "bot"
#define KEY_ATTR_COMMENT "comment"
#define KEY_ATTR_DICTATOR "dictator"
#define KEY_ATTR_EVIL "evil"
#define KEY_ATTR_GRIM "grim"
#define KEY_ATTR_GRYFFINDOR "gryffindor"
#define KEY_ATTR_HAPPY "happy"
#define KEY_ATTR_HUFFLEPUFF "hufflepuff"
#define KEY_ATTR_JOIN "join"
#define KEY_ATTR_KICKSPERFORMED "kicksperformed"
#define KEY_ATTR_KICKSRECEIVED "kicksreceived"
#define KEY_ATTR_KILL "kill"
#define KEY_ATTR_LINE "line"
#define KEY_ATTR_ME "me"
#define KEY_ATTR_PART "part"
#define KEY_ATTR_PROFANITY "profanity"
#define KEY_ATTR_QUIT "quit"
#define KEY_ATTR_RAVENCLAW "ravenclaw"
#define KEY_ATTR_REFERENCED "referenced"
#define KEY_ATTR_SAD "sad"
#define KEY_ATTR_SARCASTIC "sarcastic"
#define KEY_ATTR_SLEAZY "sleazy"
#define KEY_ATTR_SLYTHERIN "slytherin"
#define KEY_ATTR_SURPRISED "surprised"
#define KEY_ATTR_WORD "word"

/* Keys for mode maps */
#define KEY_MODE_BAN 'b'
#define KEY_MODE_HOP 'h'
#define KEY_MODE_MODERATE 'm'
#define KEY_MODE_OP 'o'
#define KEY_MODE_TOPICLOCK 't'
#define KEY_MODE_VOICE 'v'

#endif // CONSTANTS_H
