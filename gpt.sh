#!/bin/ksh

curl https://api.openai.com/v1/completions \
	-H "Content-Type: text/plain" \
	-H "Authorization: Bearer $OPENAI_API_KEY" \
	-d '{ "model": "text-davinci-002", "prompt": "Extensively describe one-eyed orc named Jesus who is currently sitting, is heavily wounded and is hungry.\n\n\n	Jesus is a one-eyed orc who is currently sitting down. He is heavily wounded and is very hungry. His one eye is swollen shut and he has multiple cuts and bruises all over his body. He is covered in dried blood and his clothes are tattered. He looks like he has been through a lot of tough battles. Despite all of his injuries, he still has a look of determination on his face. He is waiting for someone to come and help him.\n\n	I say: how can I help you, friend? I\'m not a healer, and I don\'t have any potions. Perhaps I can carry you to the nearest village.\n\n\n	He replies: I appreciate your offer, but I must stay here. I am waiting for someone.\n\n	I ask: who are you waiting for?\n\n	He answers: I am waiting for my son.", "temperature": 0.7, "max_tokens": 256, "top_p": 1, "frequency_penalty": 0, "presence_penalty": 0 }'
