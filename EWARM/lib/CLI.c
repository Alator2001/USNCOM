#include "CLI.h"
//#include "gpio.h"

typedef struct ComandsListItem
{
	const Command *commandLineDefinition;
	struct ComandsListItem *next;
} ComandsListItem;

static int help_interpreter( char *writeBuffer, size_t writeBufferLen, const char *commandString );
  

        
const Command helpCommand =
{
	.command = "help",
	.helpString = "\r\nhelp:\r\n Lists all the registered commands\r\n\r\n",
	.commandInterpreter = help_interpreter,
	.expectedNumberOfParameters = 0
};



static int8_t get_number_of_parameters( const char *commandString );



static ComandsListItem registeredCommands =
{
        .commandLineDefinition = &helpCommand,	/* The firzst command in the list is always the help command, defined in this file. */
	.next = NULL			/* The next pointer is initialised to NULL, as there are no other registered commands yet. */
};

void register_command( const Command * const commandToRegister )
{
	static ComandsListItem *lastCommandInList = &registeredCommands; //help
	ComandsListItem *newListItem;
	newListItem = ( ComandsListItem * ) malloc( sizeof( ComandsListItem ) );
	if( newListItem != NULL )
	{
		{
			newListItem->commandLineDefinition = commandToRegister;
			newListItem->next = NULL;
			lastCommandInList->next = newListItem;
			lastCommandInList = newListItem;
		}
	}
}
/*-----------------------------------------------------------*/

int process_command( const char * const commandInput, char * writeBuffer, size_t writeBufferLen  )
{
	static ComandsListItem *command = NULL;
	int returnFlag = true;
	const char *registeredCommandString;
	size_t commandStringLength;

	if( command == NULL )
	{
		for( command = &registeredCommands; command != NULL; command = command->next )
		{
			registeredCommandString = command->commandLineDefinition->command;
			commandStringLength = strlen( registeredCommandString );

			if( strncmp( commandInput, registeredCommandString, commandStringLength ) == 0 )
			{
				if(   ( commandInput[ commandStringLength ] == ' ' ) 
                                   || ( commandInput[ commandStringLength ] == 0x00 )
                                   || ( commandInput[ commandStringLength ] == '\r' )
                                   || ( commandInput[ commandStringLength ] == '\n' ))
				{
					if( command->commandLineDefinition->expectedNumberOfParameters >= 0 )
					{
						if( get_number_of_parameters( commandInput ) != command->commandLineDefinition->expectedNumberOfParameters )
						{
							returnFlag = false;
						}
					}

					break;
				}
			}
		}
	}

	if( ( command != NULL ) && ( returnFlag == false ) )
	{
		strncpy( writeBuffer, "\r\nIncorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n", writeBufferLen );
		command = NULL;
	}
	else if( command != NULL )
	{
		returnFlag = command->commandLineDefinition->commandInterpreter( writeBuffer, writeBufferLen, commandInput );
		if( returnFlag == false )
		{
			command = NULL;
		}
	}
	else
	{
		strncpy( writeBuffer, "\r\nCommand not recognised.  Enter 'help' to view a list of available commands.\r\n\r\n", writeBufferLen );
		returnFlag = false;
	}

	return returnFlag;
}
/*-----------------------------------------------------------*/

char *get_parameter( char *commandString, int wantedParameter, int *parameterStringLength )
{
	int parametersFound = 0;
	char *pcReturn = NULL;

	*parameterStringLength = 0;

	while( parametersFound < wantedParameter )
	{
		while( ( ( *commandString ) != 0x00 ) && ( ( *commandString ) != ' ' ) )
		{
			commandString++;
		}
		while( ( ( *commandString ) != 0x00 ) && ( ( *commandString ) == ' ' ) )
		{
			commandString++;
		}
		if( *commandString != 0x00 )
		{
			parametersFound++;

			if( parametersFound == wantedParameter )
			{
				pcReturn = commandString;
				while( ( ( *commandString ) != 0x00 ) && ( ( *commandString ) != ' ' ) )
				{
					( *parameterStringLength )++;
					commandString++;
				}

				if( *parameterStringLength == 0 )
				{
					pcReturn = NULL;
				}

				break;
			}
		}
		else
		{
			break;
		}
	}

	return pcReturn;
}
/*-----------------------------------------------------------*/



static int help_interpreter( char *writeBuffer, size_t writeBufferLen, const char *commandString )
{
	static ComandsListItem * command1 = NULL;
	int returnFlag;

	( void ) commandString;

	if( command1 == NULL )
	{
		command1 = &registeredCommands;
	}
	strncpy( writeBuffer, command1->commandLineDefinition->helpString, writeBufferLen );
	command1 = command1->next;

	if( command1 == NULL )
	{
		returnFlag = false;
	}
	else
	{
		returnFlag = true;
	}

	return returnFlag;
}
/*-----------------------------------------------------------*/

static int8_t get_number_of_parameters( const char *commandString )
{
	int8_t parameters = 0;
	int lastCharacterWasSpace = false;

	while( *commandString != 0x00 )
	{
		if( ( *commandString ) == ' ' )
		{
			if( lastCharacterWasSpace != true )
			{
				parameters++;
				lastCharacterWasSpace = true;
			}
		}
		else
		{
			lastCharacterWasSpace = false;
		}

		commandString++;
	}


	if( lastCharacterWasSpace == true )
	{
		parameters--;
	}


	return parameters;
}

